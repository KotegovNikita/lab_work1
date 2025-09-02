/* Nikita Kotegov st128906@student.spbu.ru Group B81 Lab_work1 */
#include "bmp_reader.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Syntax: bmpreader <bmp file>\n" << std::endl;
        return 0;
    }

    auto start_loadBMPFile_1 = std::chrono::high_resolution_clock::now();
    BMPFile* bmp_file = loadBMPFile(argv[1]);
    auto end_loadBMPFile_1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_loadBMPFile_1 = end_loadBMPFile_1 - start_loadBMPFile_1;
    std::cout << "Время выполнения loadBMPFile_1: " << duration_loadBMPFile_1.count() << " мс" << std::endl;
    
    auto start_rotated_image_counterclockwise = std::chrono::high_resolution_clock::now();
    BMPFile* rotated_image_counterclockwise = rotateBMP90Clockwise(bmp_file);
    auto end_rotated_image_counterclockwise = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_rotated_image_counterclockwise = end_rotated_image_counterclockwise - start_rotated_image_counterclockwise;
    std::cout << "Время выполнения rotated_image_counterclockwise: " << duration_rotated_image_counterclockwise.count() << " мс" << std::endl;
    
    
    auto start_save_BMPFile_1 = std::chrono::high_resolution_clock::now();
    save_BMPFile(rotated_image_counterclockwise, "rotated_image_counterclockwise.bmp");
    auto end_save_BMPFile_1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_save_BMPFile_1 = end_save_BMPFile_1 - start_save_BMPFile_1;
    std::cout << "Время выполнения save_BMPFile_1: " << duration_save_BMPFile_1.count() << " мс" << std::endl;
    
    
    auto start_rotated_image_clockwise = std::chrono::high_resolution_clock::now();
    BMPFile* rotated_image_clockwise = rotateBMP90Counterclockwise(bmp_file);
    auto end_rotated_image_clockwise = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_rotated_image_clockwise = end_rotated_image_clockwise - start_rotated_image_clockwise;
    std::cout << "Время выполнения rotated_image_clockwise: " << duration_rotated_image_clockwise.count() << " мс" << std::endl;
    
    
    auto start_save_BMPFile_2 = std::chrono::high_resolution_clock::now();
    save_BMPFile(rotated_image_clockwise, "rotated_image_clockwise.bmp");
    auto end_save_BMPFile_2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_save_BMPFile_2 = end_save_BMPFile_2 - start_save_BMPFile_2;
    std::cout << "Время выполнения save_BMPFile_2: " << duration_save_BMPFile_2.count() << " мс" << std::endl;
    
    
    int kernel_size = 5;
    float sigma = 1.8f;
    
    
    auto start_applyGaussSeparable_1 = std::chrono::high_resolution_clock::now();
    applyGaussSeparable(rotated_image_counterclockwise, kernel_size, sigma);
    auto end_applyGaussSeparable_1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_applyGaussSeparable_1 = end_applyGaussSeparable_1 - start_applyGaussSeparable_1;
    std::cout << "Время выполнения applyGaussSeparable_1: " << duration_applyGaussSeparable_1.count() << " мс" << std::endl;
    
    
    auto start_save_BMPFile_3 = std::chrono::high_resolution_clock::now();
    save_BMPFile(rotated_image_counterclockwise, "rotated_image_counterclockwise_with_gauss.bmp");
    auto end_save_BMPFile_3 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_save_BMPFile_3 = end_save_BMPFile_3 - start_save_BMPFile_3;
    std::cout << "Время выполнения save_BMPFile_3: " << duration_save_BMPFile_3.count() << " мс" << std::endl;
    
    
    auto start_applyGaussSeparable_2 = std::chrono::high_resolution_clock::now();
    applyGaussSeparable(rotated_image_clockwise, kernel_size, sigma);
    auto end_applyGaussSeparable_2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_applyGaussSeparable_2 = end_applyGaussSeparable_2 - start_applyGaussSeparable_2;
    std::cout << "Время выполнения applyGaussSeparable_2: " << duration_applyGaussSeparable_2.count() << " мс" << std::endl;
    
    
    auto start_save_BMPFile_4 = std::chrono::high_resolution_clock::now();
    save_BMPFile(rotated_image_clockwise, "rotated_image_clockwise_with_gauss.bmp");
    auto end_save_BMPFile_4 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_save_BMPFile_4 = end_save_BMPFile_4 - start_save_BMPFile_4;
    std::cout << "Время выполнения save_BMPFile_4: " << duration_save_BMPFile_4.count() << " мс" << std::endl;
    
    
    auto start_freeBMPFile_1 = std::chrono::high_resolution_clock::now();
    freeBMPFile(bmp_file);
    auto end_freeBMPFile_1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_freeBMPFile_1 = end_freeBMPFile_1 - start_freeBMPFile_1;
    std::cout << "Время выполнения freeBMPFile_1: " << duration_freeBMPFile_1.count() << " мс" << std::endl;
    
    
    auto start_freeBMPFile_2 = std::chrono::high_resolution_clock::now();
    freeBMPFile(rotated_image_counterclockwise);
    auto end_freeBMPFile_2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_freeBMPFile_2 = end_freeBMPFile_2 - start_freeBMPFile_2;
    std::cout << "Время выполнения freeBMPFile_2: " << duration_freeBMPFile_2.count() << " мс" << std::endl;
    
    
    auto start_freeBMPFile_3 = std::chrono::high_resolution_clock::now();
    freeBMPFile(rotated_image_clockwise);
    auto end_freeBMPFile_3 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_freeBMPFile_3 = end_freeBMPFile_3 - start_freeBMPFile_3;
    std::cout << "Время выполнения freeBMPFile_3: " << duration_freeBMPFile_3.count() << " мс" << std::endl;
    
    return 0;
}

// исправить исходники 
// Написать пару страниц про работу гауса в своей программе
