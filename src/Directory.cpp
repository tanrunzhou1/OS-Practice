#include "Directory.h"
#include "Inode.h"
#include "SuperBlock.h"
#include <cstring>
#include <algorithm>
#include <ctime>
#include <iostream>

Directory::Directory(char* storage, int blockSize) 
    : storage_(storage), blockSize_(blockSize) {
    maxEntriesPerBlock_ = blockSize_ / sizeof(DirEntry);
}

Directory::~Directory() {
}

bool Directory::addEntry(int dirInodeNum, const std::string& name, int inodeNum) {
    Inode dirInode;
    int inodeBlock = INODE_TABLE_BLOCK + (dirInodeNum / INODES_PER_BLOCK);
    int inodeOffset = (dirInodeNum % INODES_PER_BLOCK) * INODE_SIZE;
    std::memcpy(&dirInode, storage_ + inodeBlock * blockSize_ + inodeOffset, sizeof(Inode));
    
    if (!dirInode.isDirectory()) {
        return false;
    }
    
    DirEntry newEntry;
    newEntry.inode_num = inodeNum;
    std::strncpy(newEntry.name, name.c_str(), 27);
    newEntry.name[27] = '\0';
    
    for (int i = 0; i < MAX_DIRECT_BLOCKS; i++) {
        if (dirInode.i_block[i] == 0) {
            dirInode.i_block[i] = FIRST_DATA_BLOCK + std::rand() % (TOTAL_BLOCKS - FIRST_DATA_BLOCK);
            
            char blockBuffer[blockSize_];
            std::memset(blockBuffer, 0, blockSize_);
            std::memcpy(blockBuffer, &newEntry, sizeof(DirEntry));
            std::memcpy(storage_ + dirInode.i_block[i] * blockSize_, blockBuffer, blockSize_);
            
            dirInode.i_blocks++;
            dirInode.i_size += sizeof(DirEntry);
            dirInode.i_mtime = static_cast<uint32_t>(time(nullptr));
            std::memcpy(storage_ + inodeBlock * blockSize_ + inodeOffset, &dirInode, sizeof(Inode));
            
            return true;
        }
        
        char blockBuffer[blockSize_];
        std::memcpy(blockBuffer, storage_ + dirInode.i_block[i] * blockSize_, blockSize_);
        
        DirEntry* entries = reinterpret_cast<DirEntry*>(blockBuffer);
        for (int j = 0; j < maxEntriesPerBlock_; j++) {
            if (entries[j].inode_num == 0) {
                entries[j] = newEntry;
                std::memcpy(storage_ + dirInode.i_block[i] * blockSize_, blockBuffer, blockSize_);
                
                dirInode.i_size += sizeof(DirEntry);
                dirInode.i_mtime = static_cast<uint32_t>(time(nullptr));
                std::memcpy(storage_ + inodeBlock * blockSize_ + inodeOffset, &dirInode, sizeof(Inode));
                
                return true;
            }
        }
    }
    
    return false;
}

bool Directory::removeEntry(int dirInodeNum, const std::string& name) {
    Inode dirInode;
    int inodeBlock = INODE_TABLE_BLOCK + (dirInodeNum / INODES_PER_BLOCK);
    int inodeOffset = (dirInodeNum % INODES_PER_BLOCK) * INODE_SIZE;
    std::memcpy(&dirInode, storage_ + inodeBlock * blockSize_ + inodeOffset, sizeof(Inode));
    
    if (!dirInode.isDirectory()) {
        return false;
    }
    
    for (int i = 0; i < MAX_DIRECT_BLOCKS; i++) {
        if (dirInode.i_block[i] == 0) continue;
        
        char blockBuffer[blockSize_];
        std::memcpy(blockBuffer, storage_ + dirInode.i_block[i] * blockSize_, blockSize_);
        
        DirEntry* entries = reinterpret_cast<DirEntry*>(blockBuffer);
        for (int j = 0; j < maxEntriesPerBlock_; j++) {
            if (entries[j].inode_num != 0 && 
                std::string(entries[j].name) == name) {
                entries[j].inode_num = 0;
                entries[j].name[0] = '\0';
                std::memcpy(storage_ + dirInode.i_block[i] * blockSize_, blockBuffer, blockSize_);
                
                dirInode.i_size -= sizeof(DirEntry);
                dirInode.i_mtime = static_cast<uint32_t>(time(nullptr));
                std::memcpy(storage_ + inodeBlock * blockSize_ + inodeOffset, &dirInode, sizeof(Inode));
                
                return true;
            }
        }
    }
    
    return false;
}

int Directory::findEntry(int dirInodeNum, const std::string& name) {
    Inode dirInode;
    int inodeBlock = INODE_TABLE_BLOCK + (dirInodeNum / INODES_PER_BLOCK);
    int inodeOffset = (dirInodeNum % INODES_PER_BLOCK) * INODE_SIZE;
    std::memcpy(&dirInode, storage_ + inodeBlock * blockSize_ + inodeOffset, sizeof(Inode));
    
    if (!dirInode.isDirectory()) {
        return -1;
    }
    
    for (int i = 0; i < MAX_DIRECT_BLOCKS; i++) {
        if (dirInode.i_block[i] == 0) continue;
        
        char blockBuffer[blockSize_];
        std::memcpy(blockBuffer, storage_ + dirInode.i_block[i] * blockSize_, blockSize_);
        
        DirEntry* entries = reinterpret_cast<DirEntry*>(blockBuffer);
        for (int j = 0; j < maxEntriesPerBlock_; j++) {
            if (entries[j].inode_num != 0 && 
                std::string(entries[j].name) == name) {
                return entries[j].inode_num;
            }
        }
    }
    
    return -1;
}

std::vector<DirEntry> Directory::listEntries(int dirInodeNum) {
    std::vector<DirEntry> entries;
    
    Inode dirInode;
    int inodeBlock = INODE_TABLE_BLOCK + (dirInodeNum / INODES_PER_BLOCK);
    int inodeOffset = (dirInodeNum % INODES_PER_BLOCK) * INODE_SIZE;
    std::memcpy(&dirInode, storage_ + inodeBlock * blockSize_ + inodeOffset, sizeof(Inode));
    
    if (!dirInode.isDirectory()) {
        return entries;
    }
    
    for (int i = 0; i < MAX_DIRECT_BLOCKS; i++) {
        if (dirInode.i_block[i] == 0) continue;
        
        char blockBuffer[blockSize_];
        std::memcpy(blockBuffer, storage_ + dirInode.i_block[i] * blockSize_, blockSize_);
        
        DirEntry* blockEntries = reinterpret_cast<DirEntry*>(blockBuffer);
        for (int j = 0; j < maxEntriesPerBlock_; j++) {
            if (blockEntries[j].inode_num != 0) {
                entries.push_back(blockEntries[j]);
            }
        }
    }
    
    return entries;
}
