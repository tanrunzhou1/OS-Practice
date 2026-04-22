#include "BlockDevice.h"
#include <cstring>
#include <fstream>
#include <iostream>

BlockDevice::BlockDevice() : storage_(nullptr), is_loaded_(false) {
    storage_ = new char[TOTAL_SIZE];
    std::memset(storage_, 0, TOTAL_SIZE);
}

BlockDevice::~BlockDevice() {
    if (storage_ != nullptr) {
        delete[] storage_;
        storage_ = nullptr;
    }
}

void BlockDevice::read(int blockNum, char* buffer) {
    if (blockNum < 0 || blockNum >= TOTAL_BLOCKS) {
        std::cerr << "Error: Block number out of range: " << blockNum << std::endl;
        return;
    }
    if (buffer == nullptr) {
        std::cerr << "Error: Buffer is null" << std::endl;
        return;
    }
    std::memcpy(buffer, storage_ + blockNum * BLOCK_SIZE, BLOCK_SIZE);
}

void BlockDevice::write(int blockNum, const char* buffer) {
    if (blockNum < 0 || blockNum >= TOTAL_BLOCKS) {
        std::cerr << "Error: Block number out of range: " << blockNum << std::endl;
        return;
    }
    if (buffer == nullptr) {
        std::cerr << "Error: Buffer is null" << std::endl;
        return;
    }
    std::memcpy(storage_ + blockNum * BLOCK_SIZE, buffer, BLOCK_SIZE);
}

void BlockDevice::syncToDisk(const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return;
    }
    out.write(storage_, TOTAL_SIZE);
    out.close();
    std::cout << "File system saved to: " << filename << std::endl;
}

void BlockDevice::loadFromDisk(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        std::cerr << "Warning: Cannot open file for reading: " << filename << std::endl;
        std::cout << "Creating new file system..." << std::endl;
        return;
    }
    in.read(storage_, TOTAL_SIZE);
    in.close();
    is_loaded_ = true;
    std::cout << "File system loaded from: " << filename << std::endl;
}
