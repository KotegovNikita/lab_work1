/* Nikita Kotegov st128906@student.spbu.ru Group B81 Lab_work1 */
#include "bmp_reader.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Syntax: bmpreader <bmp file>\n" << std::endl;
        return 0;
    }

    BMPFile* bmp_file = loadBMPFile(argv[1]);
    
    BMPFile* rotated_image_counterclockwise = rotateBMP90Clockwise(bmp_file);
    save_BMPFile(rotated_image_counterclockwise, "rotated_image_counterclockwise.bmp");
    
    BMPFile* rotated_image_clockwise = rotateBMP90Counterclockwise(bmp_file);
    save_BMPFile(rotated_image_clockwise, "rotated_image_clockwise.bmp");
    
    int kernel_size = 5;
    float sigma = 1.8f;
    
    applyGaussSeparable(rotated_image_counterclockwise, kernel_size, sigma);
    save_BMPFile(rotated_image_counterclockwise, "rotated_image_counterclockwise_with_gauss.bmp");
    
    applyGaussSeparable(rotated_image_clockwise, kernel_size, sigma);
    save_BMPFile(rotated_image_clockwise, "rotated_image_clockwise_with_gauss.bmp");
    
    freeBMPFile(bmp_file);
    freeBMPFile(rotated_image_counterclockwise);
    freeBMPFile(rotated_image_clockwise);
    
    return 0;
}

// исправить исходники 
// Написать пару страниц про работу гауса в своей программе
