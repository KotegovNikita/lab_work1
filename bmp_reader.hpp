#ifndef BMP_HEADER_HPP
#define BMP_HEADER_HPP
#include <iostream>

#pragma pack(1)
struct BMPHeader {
    unsigned char ID[2];
    unsigned int file_size;
    unsigned char unused[4];
    unsigned int pixel_offset;
};

struct DIBHeader {
    unsigned int heder_size;
    unsigned int width;
    unsigned int height;
    unsigned short color_planes;
    unsigned short bits_per_pixel;
    unsigned int BI_RGB;
    unsigned int data_size;
    unsigned int pwidth;
    unsigned int pheight;
    unsigned int colors_count;
    unsigned int imp_colors_count;
};

struct BMPFile {
    BMPHeader bhdr;
    DIBHeader dhdr;
    unsigned char* data;
};
#pragma pop

void save_BMPFile(const BMPFile* bmp_file, const char* file_name);
size_t calculateBMPDataSize(const DIBHeader& dibHeader);
BMPFile* loadBMPFile(char* fname);
void freeBMPFile(BMPFile* bmp_file);
BMPFile* rotateBMP90Clockwise(const BMPFile* original);
BMPFile* rotateBMP90Counterclockwise(const BMPFile* original);
float** createGauss(int size, float sigma);

#endif
