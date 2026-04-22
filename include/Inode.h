#pragma once

#include <cstdint>
#include <string>
#include <ctime>
#include "SuperBlock.h"

const int MAX_DIRECT_BLOCKS = 10;
const int INDIRECT_BLOCK_SIZE = BLOCK_SIZE / sizeof(uint32_t);
const int MAX_FILE_SIZE = (MAX_DIRECT_BLOCKS + INDIRECT_BLOCK_SIZE) * BLOCK_SIZE;

const uint16_t S_IFREG = 0x8000;
const uint16_t S_IFDIR = 0x4000;
const uint16_t S_IRUSR = 0x0100;
const uint16_t S_IWUSR = 0x0080;
const uint16_t S_IXUSR = 0x0040;

struct Inode {
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint16_t i_links_count;
    uint16_t i_blocks;
    uint32_t i_block[MAX_DIRECT_BLOCKS];
    uint32_t i_indirect_block;

    void init(uint16_t mode);
    bool isDirectory() const;
    bool isFile() const;
    void clear();
};

class InodeManager {
private:
    char* storage_;
    int inode_count_;

public:
    InodeManager(char* storage, int inodeCount);
    ~InodeManager();

    int allocInode(uint16_t mode);
    void freeInode(int inodeNum);
    void readInode(int inodeNum, Inode& inode);
    void writeInode(int inodeNum, const Inode& inode);
    bool isAllocated(int inodeNum) const;
    int getInodeCount() const { return inode_count_; }

    int allocateBlocks(int inodeNum, int blockCount);
    void freeBlocks(int inodeNum);
};
