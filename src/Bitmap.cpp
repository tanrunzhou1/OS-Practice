#include "Bitmap.h"
#include <cstring>
#include <iostream>

Bitmap::Bitmap(int maxSize) : size_(0), max_size_(maxSize) {
    bitmap_.resize(max_size_, 0);
}

Bitmap::~Bitmap() {
}

int Bitmap::allocate() {
    for (int i = 0; i < max_size_; i++) {
        if (bitmap_[i] == 0) {
            bitmap_[i] = 1;
            size_++;
            return i;
        }
    }
    return -1;
}

int Bitmap::allocate(int index) {
    if (index < 0 || index >= max_size_) {
        std::cerr << "Error: Index out of range: " << index << std::endl;
        return -1;
    }
    if (bitmap_[index] == 1) {
        std::cerr << "Error: Index already allocated: " << index << std::endl;
        return -1;
    }
    bitmap_[index] = 1;
    size_++;
    return index;
}

void Bitmap::release(int index) {
    if (index < 0 || index >= max_size_) {
        std::cerr << "Error: Index out of range: " << index << std::endl;
        return;
    }
    if (bitmap_[index] == 1) {
        bitmap_[index] = 0;
        size_--;
    }
}

bool Bitmap::isAllocated(int index) const {
    if (index < 0 || index >= max_size_) {
        return false;
    }
    return bitmap_[index] == 1;
}

int Bitmap::getFreeCount() const {
    return max_size_ - size_;
}

void Bitmap::save(char* buffer) const {
    std::memcpy(buffer, bitmap_.data(), max_size_);
}

void Bitmap::load(const char* buffer) {
    std::memcpy(bitmap_.data(), buffer, max_size_);
    size_ = 0;
    for (int i = 0; i < max_size_; i++) {
        if (bitmap_[i] == 1) {
            size_++;
        }
    }
}

void Bitmap::setStorage(char* buffer) {
    std::memcpy(bitmap_.data(), buffer, max_size_);
    size_ = 0;
    for (int i = 0; i < max_size_; i++) {
        if (bitmap_[i] == 1) {
            size_++;
        }
    }
}

char* Bitmap::getStorage() {
    return reinterpret_cast<char*>(bitmap_.data());
}
