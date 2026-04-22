#pragma once

#include <string>
#include <vector>
#include "BlockDevice.h"
#include "SuperBlock.h"
#include "Bitmap.h"
#include "Inode.h"

struct DirEntry {
    uint32_t inode_num;
    char name[28];

    DirEntry() : inode_num(0) {
        name[0] = '\0';
    }
};

class FileSystem {
private:
    char* storage_;
    SuperBlock superBlock_;
    Bitmap* blockBitmap_;
    Bitmap* inodeBitmap_;
    InodeManager* inodeManager_;
    int currentDirInode_;

    bool initialize();

    int parsePath(const std::string& path, std::string& parentPath, std::string& fileName);
    int findInodeByPath(const std::string& path);
    int createDirectoryEntry(int parentInode, const std::string& name, int inodeNum);
    int findDirectoryEntry(int dirInode, const std::string& name);
    bool removeDirectoryEntry(int dirInode, const std::string& name);

    std::string generateRandomContent(int sizeKB);
    std::string getCurrentTimeString();

public:
    FileSystem();
    ~FileSystem();

    bool createFile(const std::string& path, int sizeKB);
    bool deleteFile(const std::string& path);
    std::string readFile(const std::string& path);
    bool copyFile(const std::string& sourcePath, const std::string& destPath);

    bool createDir(const std::string& path);
    bool deleteDir(const std::string& path);
    bool changeDir(const std::string& path);
    std::vector<DirEntry> listDir();

    std::string getSpaceInfo();

    bool save(const std::string& filename);
    bool load(const std::string& filename);

    void printWelcome();
    std::string getCurrentDir();
};
