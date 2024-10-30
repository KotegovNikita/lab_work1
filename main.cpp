#include <iostream>
#include <fstream>

struct BMPFileHeader {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BMPInfoHeader {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
};

size_t size_BMP_file(const std::string& filePath) { //Определение размера файла
    std::ifstream file(filePath, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Не удалось открыть файл.");
    }

    BMPInfoHeader info_Header;
    file.read(reinterpret_cast<char*>(&info_Header), sizeof(info_Header));

    size_t image_size = static_cast<size_t>(info_Header.biWidth) * static_cast<size_t>(std::abs(info_Header.biHeight)) *
        (info_Header.biBitCount / 8);

    return image_size;
}

uint8_t* load_file(const std::string& filePath, BMPInfoHeader& info_Header) { //Загрузка данных в память
    std::ifstream file(filePath, std::ios::binary);

    BMPFileHeader file_Header;
    file.read(reinterpret_cast<char*>(&file_Header), sizeof(file_Header));
    file.read(reinterpret_cast<char*>(&info_Header), sizeof(info_Header));

    size_t image_Size = info_Header.biWidth * std::abs(info_Header.biHeight) * 3;

    file.seekg(file_Header.bfOffBits, std::ios::beg);

    uint8_t* image_Data = new uint8_t[image_Size];
    file.read(reinterpret_cast<char*>(image_Data), image_Size);

    return image_Data;
}

void safe_File(const std::string& filePath, BMPFileHeader& file_Header, BMPInfoHeader& info_Header, uint8_t* image_Data) {
    std::ofstream file(filePath, std::ios::binary);

    file.write(reinterpret_cast<char*>(&file_Header), sizeof(file_Header));
    file.write(reinterpret_cast<char*>(&info_Header), sizeof(info_Header));
    file.write(reinterpret_cast<char*>(image_Data), info_Header.biSizeImage);
}

uint8_t* rotate_Image(uint8_t* image_Data, BMPInfoHeader& info_Header) {

}

int main() {
    std::string filePath = "file.bmp";
    BMPInfoHeader info_Header;
    uint8_t* image_Data = load_file(filePath, infoHeader);

    size_t image_size = size_BMP_file(filePath);

    std::cout << image_size << std::endl;

    delete[] image_Data;
    return 0;
}
