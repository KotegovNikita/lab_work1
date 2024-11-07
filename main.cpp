#include <iostream>
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
    
    freeBMPFile(bmp_file);
    freeBMPFile(rotated_image_counterclockwise);
    freeBMPFile(rotated_image_clockwise);
    
    return 0;
}
