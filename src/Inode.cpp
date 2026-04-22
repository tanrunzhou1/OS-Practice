#include "Inode.h"
#include "SuperBlock.h"
#include <cstring>
#include <iostream>
#include <cstdlib>

void Inode::init(uint16_t mode) {
    i_mode = mode;
    i_uid = 0;
    i_size = 0;
    i_atime = static_cast<uint32_t>(time(nullptr));
    i_ctime = static_cast<uint32_t>(time(nullptr));
    i_mtime = static_cast<uint32_t>(time(nullptr));
    i_links_count = 1;
    i_blocks = 0;
    for (int i = 0; i < MAX_DIRECT_BLOCKS; i++) {
        i_block[i] = 0;
    }
    i_indirect_block = 0;
}

bool Inode::isDirectory() const {
    return (i_mode & S_IFDIR) != 0;
}

bool Inode::isFile() const {
    return (i_mode & S_IFREG) != 0;
}

void Inode::clear() {
    std::memset(this, 0, sizeof(Inode));
}

InodeManager::InodeManager(char* storage, int inodeCount)
    : storage_(storage), inode_count_(inodeCount) {
}

InodeManager::~InodeManager() {
}

int InodeManager::allocInode(uint16_t mode) {
    for (int i = 0; i < inode_count_; i++) {
        Inode inode;
        readInode(i, inode);
        if (inode.i_size == 0 && inode.i_links_count == 0) {
            inode.init(mode);
            writeInode(i, inode);
            return i;
        }
    }
    return -1;
}

void InodeManager::freeInode(int inodeNum) {
    if (inodeNum < 0 || inodeNum >= inode_count_) {
        std::cerr << "Error: Inode number out of range: " << inodeNum << std::endl;
        return;
    }
    Inode inode;
    readInode(inodeNum, inode);
    freeBlocks(inodeNum);
    inode.clear();
    writeInode(inodeNum, inode);
}

void InodeManager::readInode(int inodeNum, Inode& inode) {
    if (inodeNum < 0 || inodeNum >= inode_count_) {
        std::cerr << "Error: Inode number out of range: " << inodeNum << std::endl;
        return;
    }
    int blockNum = INODE_TABLE_BLOCK + (inodeNum / INODES_PER_BLOCK);
    int offset = (inodeNum % INODES_PER_BLOCK) * INODE_SIZE;
    std::memcpy(&inode, storage_ + blockNum * BLOCK_SIZE + offset, sizeof(Inode));
}

void InodeManager::writeInode(int inodeNum, const Inode& inode) {
    if (inodeNum < 0 || inodeNum >= inode_count_) {
        std::cerr << "Error: Inode number out of range: " << inodeNum << std::endl;
        return;
    }
    int blockNum = INODE_TABLE_BLOCK + (inodeNum / INODES_PER_BLOCK);
    int offset = (inodeNum % INODES_PER_BLOCK) * INODE_SIZE;
    std::memcpy(storage_ + blockNum * BLOCK_SIZE + offset, &inode, sizeof(Inode));
}

bool InodeManager::isAllocated(int inodeNum) const {
    if (inodeNum < 0 || inodeNum >= inode_count_) {
        return false;
    }
    Inode inode;
    std::memcpy(&inode, storage_ + (INODE_TABLE_BLOCK + inodeNum / INODES_PER_BLOCK) * BLOCK_SIZE 
                   + (inodeNum % INODES_PER_BLOCK) * INODE_SIZE, sizeof(Inode));
    return inode.i_size != 0 || inode.i_links_count != 0;
}

int InodeManager::allocateBlocks(int inodeNum, int blockCount) {
    Inode inode;
    readInode(inodeNum, inode);

    int allocated = 0;
    for (int i = 0; i < MAX_DIRECT_BLOCKS && allocated < blockCount; i++) {
        if (inode.i_block[i] == 0) {
            int blockNum = FIRST_DATA_BLOCK + std::rand() % (TOTAL_BLOCKS - FIRST_DATA_BLOCK);
            inode.i_block[i] = blockNum;
            allocated++;
        }
    }

    if (allocated < blockCount && inode.i_indirect_block == 0) {
        inode.i_indirect_block = FIRST_DATA_BLOCK + std::rand() % (TOTAL_BLOCKS - FIRST_DATA_BLOCK);
    }

    if (inode.i_indirect_block != 0) {
        char indirectBuffer[BLOCK_SIZE];
        std::memset(indirectBuffer, 0, BLOCK_SIZE);
        uint32_t* indirectBlocks = reinterpret_cast<uint32_t*>(indirectBuffer);
        
        for (int i = 0; i < MAX_DIRECT_BLOCKS && allocated < blockCount; i++) {
            indirectBlocks[i] = inode.i_block[i];
        }
        
        int indirectIndex = MAX_DIRECT_BLOCKS;
        while (indirectIndex < INDIRECT_BLOCK_SIZE && allocated < blockCount) {
            indirectBlocks[indirectIndex] = FIRST_DATA_BLOCK + std::rand() % (TOTAL_BLOCKS - FIRST_DATA_BLOCK);
            indirectIndex++;
            allocated++;
        }
        
        std::memcpy(storage_ + inode.i_indirect_block * BLOCK_SIZE, indirectBuffer, BLOCK_SIZE);
    }

    inode.i_blocks = allocated;
    inode.i_size = allocated * BLOCK_SIZE;
    inode.i_mtime = static_cast<uint32_t>(time(nullptr));
    writeInode(inodeNum, inode);

    return allocated;
}

void InodeManager::freeBlocks(int inodeNum) {
    Inode inode;
    readInode(inodeNum, inode);

    for (int i = 0; i < MAX_DIRECT_BLOCKS; i++) {
        inode.i_block[i] = 0;
    }

    inode.i_indirect_block = 0;
    inode.i_blocks = 0;
    inode.i_size = 0;
    writeInode(inodeNum, inode);
}
