/* Nikita Kotegov st128906@student.spbu.ru Group B81 Lab_work1 (Parallel version)*/
#ifndef BMP_READER_HPP
#define BMP_READER_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

#include <thread>
#include <future>
#include <mutex>
#include <atomic>
#include <numeric>

// -- Структуры для BMP файла --

#pragma pack(push, 1) // Точное выравнивание полей структуры в памяти
struct BMPHeader {
    uint16_t file_type{0x4D42};    // "BM"
    uint32_t file_size{0};         // Размер файла в байтах
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t pixel_offset{0};      // Смещение до пиксельных данных
};

struct DIBHeader {
    uint32_t header_size{40};      // Размер этой структуры
    int32_t width{0};              // Ширина изображения в пикселях
    int32_t height{0};             // Высота изображения в пикселях
    uint16_t color_planes{1};
    uint16_t bits_per_pixel{24};   // Глубина цвета (24 бита)
    uint32_t BI_RGB{0};            // Без сжатия
    uint32_t data_size{0};         // Размер пиксельных данных
    int32_t h_resolution{0};
    int32_t v_resolution{0};
    uint32_t colors_in_palette{0};
    uint32_t important_colors{0};
};
#pragma pack(pop)

struct BMPFile {
    BMPHeader bhdr;
    DIBHeader dhdr;
    unsigned char* data{nullptr};
};

// -- Объявления функций --

void save_BMPFile(const BMPFile* bmp_file, const char* file_name);
size_t calculateBMPDataSize(const DIBHeader& dibHeader);
BMPFile* loadBMPFile(const char* fname);
void freeBMPFile(BMPFile* bmp_file);
BMPFile* rotateBMP90Clockwise(const BMPFile* original);
BMPFile* rotateBMP90Counterclockwise(const BMPFile* original);
std::vector<float> createGauss1D(int size, float sigma);
void applyGaussSeparable(BMPFile* bmp, int kernelSize, float sigma);

#endif //BMP_READER_HPP
