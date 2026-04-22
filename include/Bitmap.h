#pragma once

#include <cstdint>
#include <vector>
#include <string>

class Bitmap {
private:
    std::vector<uint8_t> bitmap_;
    int size_;
    int max_size_;

public:
    Bitmap(int maxSize);
    ~Bitmap();

    int allocate();
    int allocate(int index);
    void release(int index);
    bool isAllocated(int index) const;
    int getSize() const { return size_; }
    int getMaxSize() const { return max_size_; }
    int getFreeCount() const;

    void save(char* buffer) const;
    void load(const char* buffer);

    void setStorage(char* buffer);
    char* getStorage();
};
