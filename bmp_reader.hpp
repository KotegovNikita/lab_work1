/* Nikita Kotegov st128906@student.spbu.ru Group B81 Lab_work1 */
#ifndef BMP_HEADER_HPP
#define BMP_HEADER_HPP
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdlib>

#pragma pack(1)
struct BMPHeader { // Structure that contains file information (identifier, size, unused bytes, offset to pixel data)
    unsigned char ID[2];
    unsigned int file_size;
    unsigned char unused[4];
    unsigned int pixel_offset;
};

struct DIBHeader { // Structure that contains detailed information about the image
    unsigned int heder_size; // Header size
    unsigned int width; // Image dimensions
    unsigned int height;
    unsigned short color_planes; // Number of color planes
    unsigned short bits_per_pixel; // Bits per pixel
    unsigned int BI_RGB; // Compression type
    unsigned int data_size; // Size of the image data
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


void save_BMPFile(const BMPFile* bmp_file, const char* file_name);   // Saves the BMPFile structure to a file
size_t calculateBMPDataSize(const DIBHeader& dibHeader);             // Calculates the size of the image data, considering alignment
BMPFile* loadBMPFile(char* fname);                                   // Loads a BMP file from disk
void freeBMPFile(BMPFile* bmp_file);                                 // Frees the allocated memory for pixel data and the BMPFile structure

BMPFile* rotateBMP90Clockwise(const BMPFile* original);              // Rotates the image 90 degrees clockwise
BMPFile* rotateBMP90Counterclockwise(const BMPFile* original);       // Rotates the image 90 degrees counterclockwise

std::vector<float> createGauss1D(int size, float sigma);             // Creates a 1D Gaussian kernel
void applyGaussSeparable(BMPFile* bmp, int kernelSize, float sigma); // Applies Gaussian blur using separable convolution


#endif
