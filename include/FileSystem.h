#pragma once

#include <string>
#include <vector>
#include "BlockDevice.h"
#include "SuperBlock.h"
#include "Bitmap.h"
#include "Inode.h"
#include "Directory.h"

class FileSystem {
private:
    char* storage_;
    SuperBlock superBlock_;
    Bitmap* blockBitmap_;
    Bitmap* inodeBitmap_;
    InodeManager* inodeManager_;
    Directory* directory_;
    int currentDirInode_;
    std::string currentDirPath_;

    bool initialize();

    int parsePath(const std::string& path, std::string& parentPath, std::string& fileName);
    int findInodeByPath(const std::string& path);

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
    void dir();

    std::string getSpaceInfo();

    bool save(const std::string& filename);
    bool load(const std::string& filename);

    void printWelcome();
    std::string getCurrentDir();
};
