#include "bmp_reader.hpp"

#ifndef BMP_READER_CPP
#define BMP_READER_CPP

void save_BMPFile(const BMPFile* bmp_file, const char* file_name) {
    FILE* file = fopen(file_name, "wb");
    if (!file) {
        std::cerr << "Can't open file for writing" << std::endl;
        return;
    }
    fwrite(&bmp_file->bhdr, sizeof(BMPHeader), 1, file);
    fwrite(&bmp_file->dhdr, sizeof(DIBHeader), 1, file);
    fwrite(bmp_file->data, bmp_file->dhdr.data_size, 1, file);
    
    fclose(file);
}

size_t calculateBMPDataSize(const DIBHeader& dibHeader) {
    size_t rowSize = ((dibHeader.width * dibHeader.bits_per_pixel + 31) / 32) * 4;
    return rowSize * dibHeader.height;
}

BMPFile* loadBMPFile(char* fname) {
    FILE* file = fopen(fname, "rb");
    if (!file) {
        std::cout << "Can't load file" << std::endl;
        return 0;
    }
    BMPFile* bmp_file = (BMPFile*)malloc(sizeof(BMPFile)); //fnfkjf

    fread(&bmp_file->bhdr, sizeof(BMPHeader), 1, file);

    fread(&bmp_file->dhdr, sizeof(DIBHeader), 1, file);

    
    bmp_file->dhdr.data_size = calculateBMPDataSize(bmp_file->dhdr);
    
    bmp_file->data = (unsigned char*)malloc(bmp_file->dhdr.data_size);
    
    fseek(file, bmp_file->bhdr.pixel_offset, SEEK_SET);
    fread(bmp_file->data, bmp_file->dhdr.data_size, 1, file);
    
    fclose(file);
    
    bmp_file->dhdr.heder_size = 40;      // BITMAPINFOHEADER
    bmp_file->dhdr.BI_RGB     = 0;       // несжатый
    bmp_file->bhdr.pixel_offset = 54;    // 14 + 40
    bmp_file->bhdr.file_size = bmp_file->bhdr.pixel_offset + bmp_file->dhdr.data_size;
    
    return bmp_file;  
}



void freeBMPFile(BMPFile* bmp_file) {
    if (bmp_file) {
        if (bmp_file->data) {
            free(bmp_file->data);
        }
        free(bmp_file);
    }
}

BMPFile* rotateBMP90Clockwise(const BMPFile* original) {
    int bytesPerPixel = original->dhdr.bits_per_pixel / 8;
    
    // Создаём структуру для результата
    BMPFile* rotated = (BMPFile*)malloc(sizeof(BMPFile));
    rotated->bhdr = original->bhdr;
    rotated->dhdr = original->dhdr;
    
    // Меняем ширину/высоту
    rotated->dhdr.width  = original->dhdr.height;
    rotated->dhdr.height = original->dhdr.width;
    
    // Пересчитываем размеры
    rotated->dhdr.data_size         = calculateBMPDataSize(rotated->dhdr);
    rotated->bhdr.pixel_offset      = sizeof(BMPHeader) + sizeof(DIBHeader);
    rotated->bhdr.file_size         = rotated->bhdr.pixel_offset + rotated->dhdr.data_size;
    rotated->data                   = (unsigned char*)malloc(rotated->dhdr.data_size);
    
    // Размер одной строки в старом и новом BMP
    size_t original_size = ((original->dhdr.width * original->dhdr.bits_per_pixel + 31) / 32) * 4;
    size_t rotate_size   = ((rotated->dhdr.width * rotated->dhdr.bits_per_pixel + 31) / 32) * 4;

    // Пиксельный цикл
    for (int y = 0; y < original->dhdr.height; y++) {
        for (int x = 0; x < original->dhdr.width; x ++) {

            // 1. Читаем из исходного "снизу-вверх"
            int realY = original->dhdr.height - 1 - y;
            int original_index = realY * original_size + x * bytesPerPixel;

            // 2. Координаты 90° clockwise (логический поворот):
            int newX = y;
            int newY = (original->dhdr.width - 1) - x;

            // 3. Записываем тоже "снизу-вверх"
            int realNewY = rotated->dhdr.height - 1 - newY;
            int rotate_index = realNewY * rotate_size + newX * bytesPerPixel;

            // 4. Копируем пиксель (bytesPerPixel байт)
            for (int c = 0; c < bytesPerPixel; c++) {
                rotated->data[rotate_index + c] = original->data[original_index + c];
            }
        }
    }
    return rotated;
}


BMPFile* rotateBMP90Counterclockwise(const BMPFile* original) {
    int bytesPerPixel = original->dhdr.bits_per_pixel / 8;
    BMPFile* rotated = (BMPFile*)malloc(sizeof(BMPFile));
    rotated->bhdr = original->bhdr;
    rotated->dhdr = original->dhdr;
    
    rotated->dhdr.width = original->dhdr.height;
    rotated->dhdr.height = original->dhdr.width;
    
    rotated->dhdr.data_size = calculateBMPDataSize(rotated->dhdr);
    
    rotated->bhdr.pixel_offset = sizeof(BMPHeader) + sizeof(DIBHeader);
    rotated->bhdr.file_size    = rotated->bhdr.pixel_offset + rotated->dhdr.data_size;

    rotated->data = (unsigned char*)malloc(rotated->dhdr.data_size);
    
    size_t original_size = ((original->dhdr.width * original->dhdr.bits_per_pixel + 31) / 32) * 4;
    size_t rotate_size = ((rotated->dhdr.width * rotated->dhdr.bits_per_pixel + 31) / 32) * 4;

    for (int y = 0; y < original->dhdr.height; y++) {
        for (int x = 0; x < original->dhdr.width; x ++) {
            int original_index = (original->dhdr.height - 1 - y) * original_size + x * bytesPerPixel;
            
            int new_x = (original->dhdr.height - 1) - y;
            int new_y = x;
            int rotate_index = new_y * rotate_size + new_x * bytesPerPixel;
            
            for (int c = 0; c < bytesPerPixel; c++) {
                rotated->data[rotate_index + c] = original->data[original_index + c];
            }
        }
    }
    return rotated;
}

float** createGauss(int size, float sigma) {
    float** kernel = (float**)malloc(size * sizeof(float*)); //memory allocation for the array of pointers to kernel strings
    for (int i = 0; i < size; ++i) {
        kernel[i] = (float*)malloc(size * sizeof(float)); //memory allocation for each kernel core
    }
    float sum = 0.0f; //variable for accumulating the sum of values in the kernel
    int halfsize = size / 2; //calculate half the kernel size
    
    for (int y = -halfsize; y <= halfsize; ++y) { //vertical cycle of the Gaussian kernel
        for (int x = -halfsize; x <= halfsize; ++x) { //cycle along the horizontal of the Gaussian kernel
            float value = exp(-(x*x + y*y) / (2.0f * sigma * sigma)); //calculate the value for each element of the kernel by the formula
            kernel[y + halfsize][x + halfsize] = value; //write the calculated value into the corresponding cell of the kernel
            sum += value; //accumulation of the sum of all kernel values
        }
    }
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            kernel[y][x] /= sum; //normalize each element of the kernel
        }
    }
    return kernel;
}

void apply_Gauss(BMPFile* bmp_file, float** kernel, int kernel_size) {
    int width = bmp_file->dhdr.width;
    int height = bmp_file->dhdr.height;
    
    unsigned char* new_data = (unsigned char*)malloc(bmp_file->dhdr.data_size); //memory allocation for a new array of pixels with processed data
    
    int half_kernel_size = kernel_size / 2;
    int rowSize = ((width * bmp_file->dhdr.bits_per_pixel + 31) / 32) * 4; //determining the size of the image data string
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float new_red = 0.0f, new_green = 0.0f, new_blue = 0.0f; //Variables for new color channel values
            for (int ky = -half_kernel_size; ky <= half_kernel_size; ++ky) {
                for (int kx = -half_kernel_size; kx <= half_kernel_size; ++kx) { //cycle to traverse all kernel pixels
                    int nx = std::min(std::max(x + kx, 0), width - 1);
                    int ny = std::min(std::max(y + ky, 0), height - 1);
                    
                    int index = (ny * rowSize + nx * 3);
                    new_red += bmp_file->data[index] * kernel[ky + half_kernel_size][kx + half_kernel_size];   
                    new_green += bmp_file->data[index + 1] * kernel[ky + half_kernel_size][kx + half_kernel_size];
                    new_blue += bmp_file->data[index + 2] * kernel[ky + half_kernel_size][kx + half_kernel_size]; 
                }        
            }
        int new_index = y * rowSize + x * 3;
        new_data[new_index] = std::min(std::max(int(new_red), 0), 255);
        new_data[new_index + 1] = std::min(std::max(int(new_green), 0), 255);
        new_data[new_index + 2] = std::min(std::max(int(new_blue), 0), 255);       
        }
    }
    
    free(bmp_file->data);
    bmp_file->data = new_data;
}

void free_gauss_kernel(float** kernel, int size) {
    for (int i = 0; i < size; ++i) {
        free(kernel[i]);
    }
    free(kernel);
}

#endif
