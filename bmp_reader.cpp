/* Nikita Kotegov st128906@student.spbu.ru Group B81 Lab_work1 */
#include "bmp_reader.hpp"

#ifndef BMP_READER_CPP
#define BMP_READER_CPP

// Function to save a BMP file to disk
void save_BMPFile(const BMPFile* bmp_file, const char* file_name) {
    FILE* file = fopen(file_name, "wb");                                // Open the file in binary write mode
    if (!file) {
        std::cerr << "Can't open file for writing" << std::endl;        // Error if file cannot be opened
        return;
    }
    fwrite(&bmp_file->bhdr, sizeof(BMPHeader), 1, file);                // Write BMP header to the file
    fwrite(&bmp_file->dhdr, sizeof(DIBHeader), 1, file);                // Write DIB header to the file
    fwrite(bmp_file->data, bmp_file->dhdr.data_size, 1, file);          // Write pixel data to the file
    
    fclose(file); // Close the file
}

// Function to calculate the size of BMP pixel data
size_t calculateBMPDataSize(const DIBHeader& dibHeader) { 
    size_t rowSize = ((dibHeader.width * dibHeader.bits_per_pixel + 31) / 32) * 4; // Calculate row size with padding
    return rowSize * dibHeader.height;                                 // Return total data size (row size * height)
}

// Function to load a BMP file from disk
BMPFile* loadBMPFile(char* fname) {
    FILE* file = fopen(fname, "rb"); // Open the file in binary read mode
    if (!file) {
        std::cout << "Can't load file" << std::endl;
        return 0;
    }
    BMPFile* bmp_file = (BMPFile*)malloc(sizeof(BMPFile));             // Allocate memory for BMPFile structure

    fread(&bmp_file->bhdr, sizeof(BMPHeader), 1, file);                // Read BMP header
    fread(&bmp_file->dhdr, sizeof(DIBHeader), 1, file);                // Read DIB header

    bmp_file->dhdr.data_size = calculateBMPDataSize(bmp_file->dhdr);   // Calculate pixel data size
    bmp_file->data = (unsigned char*)malloc(bmp_file->dhdr.data_size); // Allocate memory for pixel data
    
    fseek(file, bmp_file->bhdr.pixel_offset, SEEK_SET);                // Move to the start of pixel data
    fread(bmp_file->data, bmp_file->dhdr.data_size, 1, file);          // Read pixel data
    
    fclose(file);
    
    // Set additional header fields
    bmp_file->dhdr.heder_size = 40;      // BITMAPINFOHEADER size
    bmp_file->dhdr.BI_RGB     = 0;       // Uncompressed format
    bmp_file->bhdr.pixel_offset = 54;    // Offset to pixel data (14 + 40)
    bmp_file->bhdr.file_size = bmp_file->bhdr.pixel_offset + bmp_file->dhdr.data_size; // Total file size
    
    return bmp_file; // Return the loaded BMP file
}

// Function to free memory allocated for a BMP file
void freeBMPFile(BMPFile* bmp_file) {
    if (bmp_file) {
        if (bmp_file->data) {
            free(bmp_file->data); // Free pixel data
        }
        free(bmp_file); // Free BMPFile structure
    }
}

// Function to rotate a BMP file 90 degrees clockwise
BMPFile* rotateBMP90Clockwise(const BMPFile* original) {
    int bytesPerPixel = original->dhdr.bits_per_pixel / 8;                // Calculate bytes per pixel
    
    // Allocate memory for the rotated BMP file
    BMPFile* rotated = (BMPFile*)malloc(sizeof(BMPFile));
    rotated->bhdr = original->bhdr;                                       // Copy BMP header
    rotated->dhdr = original->dhdr;                                       // Copy DIB header
    
    // Swap width and height for the rotated image
    rotated->dhdr.width  = original->dhdr.height;
    rotated->dhdr.height = original->dhdr.width;
    
    // Recalculate data size and file size
    rotated->dhdr.data_size = calculateBMPDataSize(rotated->dhdr);
    rotated->bhdr.pixel_offset = sizeof(BMPHeader) + sizeof(DIBHeader);
    rotated->bhdr.file_size = rotated->bhdr.pixel_offset + rotated->dhdr.data_size;
    rotated->data = (unsigned char*)malloc(rotated->dhdr.data_size);          // Allocate memory for rotated pixel data
    
    // Calculate row sizes for original and rotated images
    size_t original_size = ((original->dhdr.width * original->dhdr.bits_per_pixel + 31) / 32) * 4;
    size_t rotate_size   = ((rotated->dhdr.width * rotated->dhdr.bits_per_pixel + 31) / 32) * 4;

    // Rotate the image pixel by pixel
    for (unsigned int y = 0; y < original->dhdr.height; y++) {
        for (unsigned int x = 0; x < original->dhdr.width; x ++) {
            int realY = original->dhdr.height - 1 - y;                        // Flip Y coordinate
            int original_index = realY * original_size + x * bytesPerPixel;   // Calculate original pixel index

            int newX = y;                                                     // New X coordinate after rotation
            int newY = (original->dhdr.width - 1) - x;                        // New Y coordinate after rotation
            int realNewY = rotated->dhdr.height - 1 - newY;                   // Flip Y coordinate for rotated image
            int rotate_index = realNewY * rotate_size + newX * bytesPerPixel; // Calculate rotated pixel index

            // Copy pixel data (bytesPerPixel bytes)
            for (int c = 0; c < bytesPerPixel; c++) {
                rotated->data[rotate_index + c] = original->data[original_index + c];
            }
        }
    }
    return rotated; // Return the rotated BMP file
}

// Function to rotate a BMP file 90 degrees counterclockwise
BMPFile* rotateBMP90Counterclockwise(const BMPFile* original) {
    int bytesPerPixel = original->dhdr.bits_per_pixel / 8;           // Calculate bytes per pixel
    BMPFile* rotated = (BMPFile*)malloc(sizeof(BMPFile));            // Allocate memory for the rotated BMP file
    rotated->bhdr = original->bhdr;                                  // Copy BMP header
    rotated->dhdr = original->dhdr;                                  // Copy DIB header
    
    // Swap width and height for the rotated image
    rotated->dhdr.width = original->dhdr.height;
    rotated->dhdr.height = original->dhdr.width;
    
    // Recalculate data size and file size
    rotated->dhdr.data_size = calculateBMPDataSize(rotated->dhdr);
    rotated->bhdr.pixel_offset = sizeof(BMPHeader) + sizeof(DIBHeader);
    rotated->bhdr.file_size    = rotated->bhdr.pixel_offset + rotated->dhdr.data_size;

    rotated->data = (unsigned char*)malloc(rotated->dhdr.data_size); // Allocate memory for rotated pixel data
    
    // Calculate row sizes for original and rotated images
    size_t original_size = ((original->dhdr.width * original->dhdr.bits_per_pixel + 31) / 32) * 4;
    size_t rotate_size = ((rotated->dhdr.width * rotated->dhdr.bits_per_pixel + 31) / 32) * 4;

    // Rotate the image pixel by pixel
    for (unsigned int y = 0; y < original->dhdr.height; y++) {
        for (unsigned int x = 0; x < original->dhdr.width; x ++) {
            int original_index = (original->dhdr.height - 1 - y) * original_size + x * bytesPerPixel; // Calculate original pixel index
            
            int new_x = (original->dhdr.height - 1) - y;           // New X coordinate after rotation
            int new_y = x;                                         // New Y coordinate after rotation
            int rotate_index = new_y * rotate_size + new_x * bytesPerPixel; // Calculate rotated pixel index
            
            // Copy pixel data (bytesPerPixel bytes)
            for (int c = 0; c < bytesPerPixel; c++) {
                rotated->data[rotate_index + c] = original->data[original_index + c];
            }
        }
    }
    return rotated;                                                // Return the rotated BMP file
}

// Function to create a 1D Gaussian kernel
std::vector<float> createGauss1D(int size, float sigma) {
    std::vector<float> kernel(size);                               // Create a vector for the kernel
    int half = size / 2;                                           // Calculate half the size of the kernel
    float sum = 0.0f;                                              // Initialize sum for normalization
    for (int i = 0; i < size; i++) {
        int x = i - half;                                          // Calculate distance from the center
        float value = std::exp(-(x * x) / (2.0f * sigma * sigma)); // Calculate Gaussian value
        kernel[i] = value;                                         // Store value in the kernel
        sum += value;                                              // Accumulate sum for normalization
    }
    for (auto &v : kernel) {
        v /= sum;                                                  // Normalize the kernel
    }
    return kernel;                                                 // Return the Gaussian kernel
}

// Function to apply Gaussian blur using separable convolution
void applyGaussSeparable(BMPFile* bmp, int kernelSize, float sigma) {
    int width = bmp->dhdr.width;                                      // Get image width
    int height = bmp->dhdr.height;                                    // Get image height
    int rowSize = ((width * bmp->dhdr.bits_per_pixel + 31) / 32) * 4; // Calculate row size with padding
    auto kernel = createGauss1D(kernelSize, sigma);                   // Create Gaussian kernel
    int half = kernelSize / 2;                                        // Calculate half the size of the kernel

    // Temporary buffer for horizontal processing
    unsigned char* temp = (unsigned char*)malloc(bmp->dhdr.data_size);

    // Horizontal convolution
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float red = 0, green = 0, blue = 0;                   // Initialize color channels
            for (int k = -half; k <= half; k++) {
                int nx = std::min(std::max(x + k, 0), width - 1); // Handle edge cases
                int idx = y * rowSize + nx * 3;                   // Calculate pixel index
                red   += bmp->data[idx]     * kernel[k + half];   // Apply kernel to red channel
                green += bmp->data[idx + 1] * kernel[k + half];   // Apply kernel to green channel
                blue  += bmp->data[idx + 2] * kernel[k + half];   // Apply kernel to blue channel
            }
            int idx = y * rowSize + x * 3;                             // Calculate destination index
            temp[idx]     = std::min(std::max(int(red), 0), 255);      // Clamp and store red channel
            temp[idx + 1] = std::min(std::max(int(green), 0), 255);    // Clamp and store green channel
            temp[idx + 2] = std::min(std::max(int(blue), 0), 255);     // Clamp and store blue channel
        }
    }

    // Vertical convolution
    unsigned char* newData = (unsigned char*)malloc(bmp->dhdr.data_size);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float red = 0, green = 0, blue = 0;                    // Initialize color channels
            for (int k = -half; k <= half; k++) {
                int ny = std::min(std::max(y + k, 0), height - 1); // Handle edge cases
                int idx = ny * rowSize + x * 3;                    // Calculate pixel index
                red   += temp[idx]     * kernel[k + half];         // Apply kernel to red channel
                green += temp[idx + 1] * kernel[k + half];         // Apply kernel to green channel
                blue  += temp[idx + 2] * kernel[k + half];         // Apply kernel to blue channel
            }
            int idx = y * rowSize + x * 3;                             // Calculate destination index
            newData[idx]     = std::min(std::max(int(red), 0), 255);   // Clamp and store red channel
            newData[idx + 1] = std::min(std::max(int(green), 0), 255); // Clamp and store green channel
            newData[idx + 2] = std::min(std::max(int(blue), 0), 255);  // Clamp and store blue channel
        }
    }

    free(bmp->data);             // Free original pixel data
    bmp->data = newData;         // Replace with blurred data
    free(temp);                  // Free temporary buffer
}

#endif
