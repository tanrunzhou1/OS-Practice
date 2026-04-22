#pragma once

#include <cstdint>
#include <string>

class BlockDevice {
private:
    static const int BLOCK_SIZE = 1024;
    static const int TOTAL_BLOCKS = 16384;
    static const int TOTAL_SIZE = 16777216;

    char* storage_;
    bool is_loaded_;

public:
    BlockDevice();
    ~BlockDevice();

    void read(int blockNum, char* buffer);
    void write(int blockNum, const char* buffer);
    void syncToDisk(const std::string& filename);
    void loadFromDisk(const std::string& filename);

    int getBlockSize() const { return BLOCK_SIZE; }
    int getTotalBlocks() const { return TOTAL_BLOCKS; }
    int getTotalSize() const { return TOTAL_SIZE; }
    bool isLoaded() const { return is_loaded_; }

    char* getStorage() { return storage_; }
};
