#include <iostream>
#include <fstream>
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
    BMPFile* rotated = (BMPFile*)malloc(sizeof(BMPFile));
    rotated->bhdr = original->bhdr;
    rotated->dhdr = original->dhdr;
    
    rotated->dhdr.width = original->dhdr.height;
    rotated->dhdr.height = original->dhdr.width;
    
    rotated->dhdr.data_size = calculateBMPDataSize(rotated->dhdr);
    rotated->data = (unsigned char*)malloc(rotated->dhdr.data_size);
    
    size_t original_size = ((original->dhdr.width * original->dhdr.bits_per_pixel + 31) / 32) * 4;
    size_t rotate_size = ((rotated->dhdr.width * rotated->dhdr.bits_per_pixel + 31) / 32) * 4;

    for (int y = 0; y < original->dhdr.height; y++) {
        for (int x = 0; x < original->dhdr.width; x ++) {
        int original_index = y * original_size + x * 3;
        
        int new_x = original->dhdr.height - 1 - y;
        int new_y = x;
        int rotate_index = new_y * rotate_size + new_x * 3;
        
        rotated->data[rotate_index] = original->data[original_index];
        rotated->data[rotate_index + 1] = original->data[original_index + 1];
        rotated->data[rotate_index + 2] = original->data[original_index + 2];
        }
    }
    return rotated;
}

BMPFile* rotateBMP90Counterclockwise(const BMPFile* original) {
    BMPFile* rotated = (BMPFile*)malloc(sizeof(BMPFile));
    rotated->bhdr = original->bhdr;
    rotated->dhdr = original->dhdr;
    
    rotated->dhdr.width = original->dhdr.height;
    rotated->dhdr.height = original->dhdr.width;
    
    rotated->dhdr.data_size = calculateBMPDataSize(rotated->dhdr);
    rotated->data = (unsigned char*)malloc(rotated->dhdr.data_size);
    
    size_t original_size = ((original->dhdr.width * original->dhdr.bits_per_pixel + 31) / 32) * 4;
    size_t rotate_size = ((rotated->dhdr.width * rotated->dhdr.bits_per_pixel + 31) / 32) * 4;

    for (int y = 0; y < original->dhdr.height; y++) {
        for (int x = 0; x < original->dhdr.width; x ++) {
        int original_index = y * original_size + x * 3;
        
        int new_x = y;
        int new_y = original->dhdr.width - 1 - x;
        int rotate_index = new_y * rotate_size + new_x * 3;
        
        rotated->data[rotate_index] = original->data[original_index];
        rotated->data[rotate_index + 1] = original->data[original_index + 1];
        rotated->data[rotate_index + 2] = original->data[original_index + 2];
        }
    }
    return rotated;
}

#endif
