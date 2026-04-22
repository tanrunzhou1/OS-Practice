#include "FileSystem.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>

FileSystem::FileSystem() : storage_(nullptr), blockBitmap_(nullptr), 
                           inodeBitmap_(nullptr), inodeManager_(nullptr),
                           directory_(nullptr), currentDirInode_(0), currentDirPath_("/") {
    storage_ = new char[TOTAL_SIZE];
    std::memset(storage_, 0, TOTAL_SIZE);
    
    blockBitmap_ = new Bitmap(TOTAL_BLOCKS);
    inodeBitmap_ = new Bitmap(TOTAL_INODES);
    inodeManager_ = new InodeManager(storage_, TOTAL_INODES);
    directory_ = new Directory(storage_, BLOCK_SIZE);
    
    initialize();
}

FileSystem::~FileSystem() {
    if (directory_) delete directory_;
    if (storage_) delete[] storage_;
    if (blockBitmap_) delete blockBitmap_;
    if (inodeBitmap_) delete inodeBitmap_;
    if (inodeManager_) delete inodeManager_;
}

bool FileSystem::initialize() {
    superBlock_.init();
    
    char superBlockBuffer[BLOCK_SIZE];
    std::memcpy(superBlockBuffer, &superBlock_, sizeof(SuperBlock));
    
    char* device = storage_;
    std::memcpy(device + SUPERBLOCK_BLOCK * BLOCK_SIZE, superBlockBuffer, BLOCK_SIZE);
    
    currentDirInode_ = 0;
    
    Inode rootInode;
    rootInode.init(S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR);
    rootInode.i_block[0] = FIRST_DATA_BLOCK + std::rand() % (TOTAL_BLOCKS - FIRST_DATA_BLOCK);
    rootInode.i_blocks = 1;
    inodeManager_->writeInode(0, rootInode);
    inodeBitmap_->allocate();
    
    blockBitmap_->allocate(rootInode.i_block[0]);
    superBlock_.s_free_blocks_count--;
    
    char blockBuffer[BLOCK_SIZE];
    std::memset(blockBuffer, 0, BLOCK_SIZE);
    DirEntry* entries = reinterpret_cast<DirEntry*>(blockBuffer);
    entries[0].inode_num = 0;
    std::strncpy(entries[0].name, ".", 27);
    entries[1].inode_num = 0;
    std::strncpy(entries[1].name, "..", 27);
    std::memcpy(storage_ + rootInode.i_block[0] * BLOCK_SIZE, blockBuffer, BLOCK_SIZE);
    
    rootInode.i_size = 2 * sizeof(DirEntry);
    inodeManager_->writeInode(0, rootInode);
    
    return true;
}

int FileSystem::parsePath(const std::string& path, std::string& parentPath, std::string& fileName) {
    if (path.empty() || path[0] != '/') {
        return -1;
    }
    
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash == 0) {
        parentPath = "/";
        fileName = path.substr(1);
    } else {
        parentPath = path.substr(0, lastSlash);
        fileName = path.substr(lastSlash + 1);
    }
    
    return 0;
}

int FileSystem::findInodeByPath(const std::string& path) {
    if (path == "/" || path.empty()) {
        return 0;
    }
    
    std::vector<std::string> parts;
    std::stringstream ss(path.substr(1));
    std::string part;
    while (std::getline(ss, part, '/')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    int currentInode = 0;
    
    for (size_t i = 0; i < parts.size(); i++) {
        int foundInode = directory_->findEntry(currentInode, parts[i]);
        if (foundInode < 0) {
            return -1;
        }
        
        Inode inode;
        inodeManager_->readInode(foundInode, inode);
        
        if (i == parts.size() - 1) {
            return foundInode;
        }
        
        if (!inode.isDirectory()) {
            return -1;
        }
        
        currentInode = foundInode;
    }
    
    return currentInode;
}

std::string FileSystem::generateRandomContent(int sizeKB) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    const int charsetSize = sizeof(charset) - 1;
    
    std::string content;
    content.reserve(sizeKB * 1024);
    
    for (int i = 0; i < sizeKB * 1024; i++) {
        content += charset[std::rand() % charsetSize];
    }
    
    return content;
}

std::string FileSystem::getCurrentTimeString() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

bool FileSystem::createFile(const std::string& path, int sizeKB) {
    if (sizeKB <= 0 || sizeKB > MAX_FILE_SIZE / 1024) {
        std::cerr << "Error: File size exceeds maximum (" << MAX_FILE_SIZE / 1024 << " KB)" << std::endl;
        return false;
    }
    
    int existingInode = findInodeByPath(path);
    if (existingInode >= 0) {
        std::cerr << "Error: File already exists" << std::endl;
        return false;
    }
    
    std::string parentPath, fileName;
    if (parsePath(path, parentPath, fileName) < 0) {
        std::cerr << "Error: Invalid path" << std::endl;
        return false;
    }
    
    int parentInode = findInodeByPath(parentPath.empty() ? "/" : parentPath);
    if (parentInode < 0) {
        std::cerr << "Error: Parent directory not found" << std::endl;
        return false;
    }
    
    Inode parentDir;
    inodeManager_->readInode(parentInode, parentDir);
    if (!parentDir.isDirectory()) {
        std::cerr << "Error: Parent is not a directory" << std::endl;
        return false;
    }
    
    int inodeNum = inodeManager_->allocInode(S_IFREG | S_IRUSR | S_IWUSR);
    if (inodeNum < 0) {
        std::cerr << "Error: No free inode available" << std::endl;
        return false;
    }
    
    inodeBitmap_->allocate(inodeNum);
    superBlock_.s_free_inodes_count--;
    
    inodeManager_->allocateBlocks(inodeNum, sizeKB);
    
    for (int i = 0; i < sizeKB; i++) {
        Inode fileInode;
        inodeManager_->readInode(inodeNum, fileInode);
        
        if (i < MAX_DIRECT_BLOCKS && fileInode.i_block[i] != 0) {
            std::string content = generateRandomContent(1);
            std::memcpy(storage_ + fileInode.i_block[i] * BLOCK_SIZE, content.c_str(), BLOCK_SIZE);
        }
    }
    
    directory_->addEntry(parentInode, fileName, inodeNum);
    
    std::cout << "File created: " << path << " (" << sizeKB << " KB)" << std::endl;
    return true;
}

bool FileSystem::deleteFile(const std::string& path) {
    int inodeNum = findInodeByPath(path);
    if (inodeNum < 0) {
        std::cerr << "Error: File not found" << std::endl;
        return false;
    }
    
    Inode inode;
    inodeManager_->readInode(inodeNum, inode);
    
    if (inode.isDirectory()) {
        std::cerr << "Error: Cannot delete directory, use deleteDir" << std::endl;
        return false;
    }
    
    std::string parentPath, fileName;
    parsePath(path, parentPath, fileName);
    int parentInode = findInodeByPath(parentPath.empty() ? "/" : parentPath);
    
    if (parentInode >= 0) {
        directory_->removeEntry(parentInode, fileName);
    }
    
    inodeManager_->freeInode(inodeNum);
    inodeBitmap_->release(inodeNum);
    superBlock_.s_free_inodes_count++;
    
    std::cout << "File deleted: " << path << std::endl;
    return true;
}

std::string FileSystem::readFile(const std::string& path) {
    int inodeNum = findInodeByPath(path);
    if (inodeNum < 0) {
        std::cerr << "Error: File not found" << std::endl;
        return "";
    }
    
    Inode inode;
    inodeManager_->readInode(inodeNum, inode);
    
    if (!inode.isFile()) {
        std::cerr << "Error: Not a file" << std::endl;
        return "";
    }
    
    std::string content;
    content.reserve(inode.i_size);
    
    for (int i = 0; i < MAX_DIRECT_BLOCKS && i * BLOCK_SIZE < inode.i_size; i++) {
        if (inode.i_block[i] != 0) {
            char blockBuffer[BLOCK_SIZE];
            std::memcpy(blockBuffer, storage_ + inode.i_block[i] * BLOCK_SIZE, BLOCK_SIZE);
            content.append(blockBuffer, std::min(BLOCK_SIZE, static_cast<int>(inode.i_size - i * BLOCK_SIZE)));
        }
    }
    
    return content;
}

bool FileSystem::copyFile(const std::string& sourcePath, const std::string& destPath) {
    int sourceInode = findInodeByPath(sourcePath);
    if (sourceInode < 0) {
        std::cerr << "Error: Source file not found" << std::endl;
        return false;
    }
    
    Inode sourceInodeStruct;
    inodeManager_->readInode(sourceInode, sourceInodeStruct);
    
    if (!sourceInodeStruct.isFile()) {
        std::cerr << "Error: Source is not a file" << std::endl;
        return false;
    }
    
    return createFile(destPath, sourceInodeStruct.i_size / 1024);
}

bool FileSystem::createDir(const std::string& path) {
    if (path == "/" || path.empty()) {
        std::cerr << "Error: Cannot create root directory" << std::endl;
        return false;
    }
    
    int existingInode = findInodeByPath(path);
    if (existingInode >= 0) {
        std::cerr << "Error: Directory already exists" << std::endl;
        return false;
    }
    
    std::string parentPath, dirName;
    if (parsePath(path, parentPath, dirName) < 0) {
        std::cerr << "Error: Invalid path" << std::endl;
        return false;
    }
    
    int parentInode = findInodeByPath(parentPath.empty() ? "/" : parentPath);
    if (parentInode < 0) {
        std::cerr << "Error: Parent directory not found" << std::endl;
        return false;
    }
    
    int inodeNum = inodeManager_->allocInode(S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR);
    if (inodeNum < 0) {
        std::cerr << "Error: No free inode available" << std::endl;
        return false;
    }
    
    inodeBitmap_->allocate(inodeNum);
    superBlock_.s_free_inodes_count--;
    
    Inode newDirInode;
    newDirInode.init(S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR);
    newDirInode.i_block[0] = FIRST_DATA_BLOCK + std::rand() % (TOTAL_BLOCKS - FIRST_DATA_BLOCK);
    newDirInode.i_blocks = 1;
    newDirInode.i_size = 0;
    
    blockBitmap_->allocate(newDirInode.i_block[0]);
    superBlock_.s_free_blocks_count--;
    
    char blockBuffer[BLOCK_SIZE];
    std::memset(blockBuffer, 0, BLOCK_SIZE);
    DirEntry* entries = reinterpret_cast<DirEntry*>(blockBuffer);
    entries[0].inode_num = inodeNum;
    std::strncpy(entries[0].name, ".", 27);
    entries[1].inode_num = parentInode;
    std::strncpy(entries[1].name, "..", 27);
    std::memcpy(storage_ + newDirInode.i_block[0] * BLOCK_SIZE, blockBuffer, BLOCK_SIZE);
    
    newDirInode.i_size = 2 * sizeof(DirEntry);
    inodeManager_->writeInode(inodeNum, newDirInode);
    
    directory_->addEntry(parentInode, dirName, inodeNum);
    
    std::cout << "Directory created: " << path << std::endl;
    return true;
}

bool FileSystem::deleteDir(const std::string& path) {
    if (path == "/" || path.empty()) {
        std::cerr << "Error: Cannot delete root directory" << std::endl;
        return false;
    }
    
    int dirInode = findInodeByPath(path);
    if (dirInode < 0) {
        std::cerr << "Error: Directory not found" << std::endl;
        return false;
    }
    
    if (dirInode == currentDirInode_) {
        std::cerr << "Error: Cannot delete current working directory" << std::endl;
        return false;
    }
    
    Inode dirInodeStruct;
    inodeManager_->readInode(dirInode, dirInodeStruct);
    
    if (!dirInodeStruct.isDirectory()) {
        std::cerr << "Error: Not a directory" << std::endl;
        return false;
    }
    
    if (dirInodeStruct.i_size > 0) {
        std::cerr << "Error: Directory is not empty" << std::endl;
        return false;
    }
    
    std::string parentPath, dirName;
    parsePath(path, parentPath, dirName);
    int parentInode = findInodeByPath(parentPath.empty() ? "/" : parentPath);
    
    if (parentInode >= 0) {
        directory_->removeEntry(parentInode, dirName);
    }
    
    inodeManager_->freeInode(dirInode);
    inodeBitmap_->release(dirInode);
    superBlock_.s_free_inodes_count++;
    
    std::cout << "Directory deleted: " << path << std::endl;
    return true;
}

bool FileSystem::changeDir(const std::string& path) {
    int targetInode = findInodeByPath(path);
    if (targetInode < 0) {
        std::cerr << "Error: Directory not found" << std::endl;
        return false;
    }
    
    Inode targetInodeStruct;
    inodeManager_->readInode(targetInode, targetInodeStruct);
    
    if (!targetInodeStruct.isDirectory()) {
        std::cerr << "Error: Not a directory" << std::endl;
        return false;
    }
    
    currentDirInode_ = targetInode;
    
    if (path == "/") {
        currentDirPath_ = "/";
    } else if (path[0] == '/') {
        currentDirPath_ = path;
    } else {
        if (currentDirPath_ == "/") {
            currentDirPath_ = "/" + path;
        } else {
            currentDirPath_ = currentDirPath_ + "/" + path;
        }
    }
    
    std::cout << "Changed directory to: " << currentDirPath_ << std::endl;
    return true;
}

std::vector<DirEntry> FileSystem::listDir() {
    return directory_->listEntries(currentDirInode_);
}

void FileSystem::dir() {
    std::vector<DirEntry> entries = directory_->listEntries(currentDirInode_);
    
    int visibleCount = 0;
    for (const auto& entry : entries) {
        if (std::string(entry.name) != "." && std::string(entry.name) != "..") {
            visibleCount++;
        }
    }
    
    if (visibleCount == 0) {
        std::cout << "  (empty directory)" << std::endl;
        return;
    }
    
    std::cout << std::left << std::setw(10) << "Type" 
              << std::setw(12) << "Size" 
              << std::setw(20) << "Modified Time" 
              << "Name" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (const auto& entry : entries) {
        if (std::string(entry.name) == "." || std::string(entry.name) == "..") {
            continue;
        }
        
        Inode inode;
        inodeManager_->readInode(entry.inode_num, inode);
        
        std::string type = inode.isDirectory() ? "<DIR>" : "<FILE>";
        
        std::string size = inode.isDirectory() ? "-" : std::to_string(inode.i_size) + " B";
        
        time_t mtime = inode.i_mtime;
        struct tm* timeinfo = localtime(&mtime);
        char timeBuffer[32];
        std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        
        std::cout << std::left << std::setw(10) << type 
                  << std::setw(12) << size 
                  << std::setw(20) << timeBuffer 
                  << entry.name << std::endl;
    }
    
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "  Total: " << visibleCount << " items" << std::endl;
}

std::string FileSystem::getSpaceInfo() {
    std::stringstream ss;
    ss << "========== Storage Space Info ==========" << std::endl;
    ss << "Total Blocks:      " << superBlock_.s_blocks_count << std::endl;
    ss << "Used Blocks:       " << (superBlock_.s_blocks_count - superBlock_.s_free_blocks_count) << std::endl;
    ss << "Free Blocks:       " << superBlock_.s_free_blocks_count << std::endl;
    ss << "Total Inodes:      " << superBlock_.s_inodes_count << std::endl;
    ss << "Used Inodes:       " << (superBlock_.s_inodes_count - superBlock_.s_free_inodes_count) << std::endl;
    ss << "Free Inodes:       " << superBlock_.s_free_inodes_count << std::endl;
    ss << "=========================================" << std::endl;
    return ss.str();
}

bool FileSystem::save(const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Error: Cannot open file for writing" << std::endl;
        return false;
    }
    out.write(storage_, TOTAL_SIZE);
    out.close();
    std::cout << "File system saved to: " << filename << std::endl;
    return true;
}

bool FileSystem::load(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        std::cerr << "Warning: Cannot open file, creating new file system" << std::endl;
        return false;
    }
    in.read(storage_, TOTAL_SIZE);
    in.close();
    std::cout << "File system loaded from: " << filename << std::endl;
    return true;
}

void FileSystem::printWelcome() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Unix-style Virtual File System      " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "   Welcome to our File System!          " << std::endl;
    std::cout << "   Group Members:                      " << std::endl;
    std::cout << "   [Your Name] - [Student ID]          " << std::endl;
    std::cout << "   [Your Name] - [Student ID]          " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
}

std::string FileSystem::getCurrentDir() {
    return currentDirPath_;
}
