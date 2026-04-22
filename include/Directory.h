#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct DirEntry {
    uint32_t inode_num;
    char name[28];

    DirEntry() : inode_num(0) {
        name[0] = '\0';
    }
};

class Directory {
private:
    char* storage_;
    int blockSize_;
    int maxEntriesPerBlock_;

public:
    Directory(char* storage, int blockSize = 1024);
    ~Directory();

    bool addEntry(int dirBlock, const std::string& name, int inodeNum);
    bool removeEntry(int dirBlock, const std::string& name);
    int findEntry(int dirBlock, const std::string& name);
    std::vector<DirEntry> listEntries(int dirBlock);
};
