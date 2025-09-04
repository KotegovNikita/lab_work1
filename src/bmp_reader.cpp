/* Nikita Kotegov st128906@student.spbu.ru Group B81 Lab_work1 (Parallel version) */
#include "bmp_reader.hpp"

const unsigned int NUM_THREADS = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4;

// --- 1. Сохранение файла ---
void save_BMPFile(const BMPFile* bmp_file, const char* file_name) {
    FILE* file = fopen(file_name, "wb");
    if (!file) {
        std::cerr << "Can't open file for writing" << std::endl;
        return;
    }

    // Заголовки пишем последовательно
    fwrite(&bmp_file->bhdr, sizeof(BMPHeader), 1, file);
    fwrite(&bmp_file->dhdr, sizeof(DIBHeader), 1, file);

    fwrite(bmp_file->data, 1, bmp_file->dhdr.data_size, file);

    fclose(file);
}

// --- 2. Вычисление размера данных ---
size_t calculateBMPDataSize(const DIBHeader& dibHeader) {
    auto future = std::async(std::launch::async, [&]() {
        size_t rowSize = ((dibHeader.width * dibHeader.bits_per_pixel + 31) / 32) * 4;
        return rowSize * dibHeader.height;
    });
    return future.get();
}

// --- 3. Загрузка файла ---
BMPFile* loadBMPFile(const char* fname) {
    FILE* file = fopen(fname, "rb");
    if (!file) {
        std::cout << "Can't load file" << std::endl;
        return nullptr;
    }

    BMPFile* bmp_file = new BMPFile();

    fread(&bmp_file->bhdr, sizeof(BMPHeader), 1, file);
    fread(&bmp_file->dhdr, sizeof(DIBHeader), 1, file);

    bmp_file->dhdr.data_size = calculateBMPDataSize(bmp_file->dhdr);
    bmp_file->data = new unsigned char[bmp_file->dhdr.data_size];

    // Чтение пиксельных данных выполняется последовательно для корректности.
    fseek(file, bmp_file->bhdr.pixel_offset, SEEK_SET);
    fread(bmp_file->data, 1, bmp_file->dhdr.data_size, file);

    fclose(file);

    // Нормализация заголовков для обеспечения консистентности данных.
    // Это исправляет ошибку с некорректным смещением пикселей (pixel_offset).
    bmp_file->dhdr.header_size = 40;
    bmp_file->dhdr.BI_RGB = 0;
    bmp_file->bhdr.pixel_offset = sizeof(BMPHeader) + sizeof(DIBHeader); // 14 + 40 = 54
    bmp_file->bhdr.file_size = bmp_file->bhdr.pixel_offset + bmp_file->dhdr.data_size;

    return bmp_file;
}

// --- 4. Освобождение памяти ---
void freeBMPFile(BMPFile* bmp_file) {
    if (bmp_file) {
        delete[] bmp_file->data;
        delete bmp_file;
    }
}

// --- 5. Поворот изображения ---
// Идея: Разделить строки изображения между потоками. Каждый поток обрабатывает свой
// диапазон строк.
BMPFile* rotateBMP90Clockwise(const BMPFile* original) {
    BMPFile* rotated = new BMPFile();
    rotated->bhdr = original->bhdr;
    rotated->dhdr = original->dhdr;

    rotated->dhdr.width = original->dhdr.height;
    rotated->dhdr.height = original->dhdr.width;
    rotated->dhdr.data_size = calculateBMPDataSize(rotated->dhdr);
    rotated->bhdr.file_size = rotated->bhdr.pixel_offset + rotated->dhdr.data_size;
    rotated->data = new unsigned char[rotated->dhdr.data_size];

    int bytesPerPixel = original->dhdr.bits_per_pixel / 8;
    size_t original_size = ((original->dhdr.width * original->dhdr.bits_per_pixel + 31) / 32) * 4;
    size_t rotate_size = ((rotated->dhdr.width * rotated->dhdr.bits_per_pixel + 31) / 32) * 4;

    std::vector<std::thread> threads;
    int32_t height = original->dhdr.height;
    int32_t rows_per_thread = height / NUM_THREADS;

    for (unsigned int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([=]() {
            int32_t start_y = i * rows_per_thread;
            int32_t end_y = (i == NUM_THREADS - 1) ? height : start_y + rows_per_thread;

            for (int32_t y = start_y; y < end_y; y++) {
                for (int32_t x = 0; x < original->dhdr.width; x++) {
                    int original_index = (original->dhdr.height - 1 - y) * original_size + x * bytesPerPixel;
                    int newX = y;
                    int newY = (original->dhdr.width - 1) - x;
                    int rotate_index = (rotated->dhdr.height - 1 - newY) * rotate_size + newX * bytesPerPixel;

                    for (int c = 0; c < bytesPerPixel; c++) {
                        rotated->data[rotate_index + c] = original->data[original_index + c];
                    }
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }
    return rotated;
}

BMPFile* rotateBMP90Counterclockwise(const BMPFile* original) {
    BMPFile* rotated = new BMPFile();
    rotated->bhdr = original->bhdr;
    rotated->dhdr = original->dhdr;

    rotated->dhdr.width = original->dhdr.height;
    rotated->dhdr.height = original->dhdr.width;
    rotated->dhdr.data_size = calculateBMPDataSize(rotated->dhdr);
    rotated->bhdr.file_size = rotated->bhdr.pixel_offset + rotated->dhdr.data_size;
    rotated->data = new unsigned char[rotated->dhdr.data_size];
    
    int bytesPerPixel = original->dhdr.bits_per_pixel / 8;
    size_t original_size = ((original->dhdr.width * original->dhdr.bits_per_pixel + 31) / 32) * 4;
    size_t rotate_size = ((rotated->dhdr.width * rotated->dhdr.bits_per_pixel + 31) / 32) * 4;

    std::vector<std::thread> threads;
    int32_t height = original->dhdr.height;
    int32_t rows_per_thread = height / NUM_THREADS;

    for (unsigned int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([=]() {
            int32_t start_y = i * rows_per_thread;
            int32_t end_y = (i == NUM_THREADS - 1) ? height : start_y + rows_per_thread;
            for (int32_t y = start_y; y < end_y; y++) {
                for (int32_t x = 0; x < original->dhdr.width; x++) {
                    int original_index = (original->dhdr.height - 1 - y) * original_size + x * bytesPerPixel;
                    int new_x = (original->dhdr.height - 1) - y;
                    int new_y = x;
                    int rotate_index = (rotated->dhdr.height - 1 - new_y) * rotate_size + new_x * bytesPerPixel;
                    for (int c = 0; c < bytesPerPixel; c++) {
                        rotated->data[rotate_index + c] = original->data[original_index + c];
                    }
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }
    return rotated;
}

// --- 6. Создание ядра Гаусса ---
// Идея: Разделить вычисление весов ядра между потоками.
std::vector<float> createGauss1D(int size, float sigma) {
    std::vector<float> kernel(size);
    int half = size / 2;
    std::vector<float> partial_sums(NUM_THREADS, 0.0f);

    std::vector<std::thread> threads;
    int items_per_thread = size / NUM_THREADS;

    // Шаг 1: Параллельное вычисление ненормализованных значений и частичных сумм
    for (unsigned int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&, i]() {
            int start = i * items_per_thread;
            int end = (i == NUM_THREADS - 1) ? size : start + items_per_thread;
            float local_sum = 0.0f;
            for (int j = start; j < end; j++) {
                int x = j - half;
                float value = std::exp(-(x * x) / (2.0f * sigma * sigma));
                kernel[j] = value;
                local_sum += value;
            }
            partial_sums[i] = local_sum;
        });
    }
    for (auto& t : threads) { t.join(); }

    // Шаг 2: Суммирование
    float total_sum = std::accumulate(partial_sums.begin(), partial_sums.end(), 0.0f);

    // Шаг 3: Параллельная нормализация
    threads.clear();
    for (unsigned int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&, i, total_sum]() {
            int start = i * items_per_thread;
            int end = (i == NUM_THREADS - 1) ? size : start + items_per_thread;
            for (int j = start; j < end; j++) {
                kernel[j] /= total_sum;
            }
        });
    }
    for (auto& t : threads) { t.join(); }

    return kernel;
}

// --- 7. Применение Гауссова размытия ---
// Идея: Горизонтальный и вертикальный проходы свертки распараллеливаются
// путем разделения строк изображения между потоками.
void applyGaussSeparable(BMPFile* bmp, int kernelSize, float sigma) {
    int width = bmp->dhdr.width;
    int height = bmp->dhdr.height;
    int rowSize = ((width * bmp->dhdr.bits_per_pixel + 31) / 32) * 4;
    auto kernel = createGauss1D(kernelSize, sigma);
    int half = kernelSize / 2;

    unsigned char* temp = new unsigned char[bmp->dhdr.data_size];
    unsigned char* newData = new unsigned char[bmp->dhdr.data_size];

    std::vector<std::thread> threads;
    int rows_per_thread = height / NUM_THREADS;

    // --- Горизонтальный проход ---
    for (unsigned int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([=]() {
            int start_y = i * rows_per_thread;
            int end_y = (i == NUM_THREADS - 1) ? height : start_y + rows_per_thread;
            for (int y = start_y; y < end_y; y++) {
                for (int x = 0; x < width; x++) {
                    float r = 0, g = 0, b = 0;
                    for (int k = -half; k <= half; k++) {
                        int nx = std::min(std::max(x + k, 0), width - 1);
                        int idx = y * rowSize + nx * 3;
                        r += bmp->data[idx] * kernel[k + half];
                        g += bmp->data[idx + 1] * kernel[k + half];
                        b += bmp->data[idx + 2] * kernel[k + half];
                    }
                    int dest_idx = y * rowSize + x * 3;
                    temp[dest_idx] = std::min(std::max(int(r), 0), 255);
                    temp[dest_idx + 1] = std::min(std::max(int(g), 0), 255);
                    temp[dest_idx + 2] = std::min(std::max(int(b), 0), 255);
                }
            }
        });
    }
    for (auto& t : threads) { t.join(); }

    // --- Вертикальный проход ---
    threads.clear();
    for (unsigned int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([=]() {
            int start_y = i * rows_per_thread;
            int end_y = (i == NUM_THREADS - 1) ? height : start_y + rows_per_thread;
            for (int y = start_y; y < end_y; y++) {
                for (int x = 0; x < width; x++) {
                    float r = 0, g = 0, b = 0;
                    for (int k = -half; k <= half; k++) {
                        int ny = std::min(std::max(y + k, 0), height - 1);
                        int idx = ny * rowSize + x * 3;
                        r += temp[idx] * kernel[k + half];
                        g += temp[idx + 1] * kernel[k + half];
                        b += temp[idx + 2] * kernel[k + half];
                    }
                    int dest_idx = y * rowSize + x * 3;
                    newData[dest_idx] = std::min(std::max(int(r), 0), 255);
                    newData[dest_idx + 1] = std::min(std::max(int(g), 0), 255);
                    newData[dest_idx + 2] = std::min(std::max(int(b), 0), 255);
                }
            }
        });
    }
    for (auto& t : threads) { t.join(); }

    delete[] bmp->data;
    bmp->data = newData;
    delete[] temp;
}

