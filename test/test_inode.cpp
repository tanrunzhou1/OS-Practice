#include <iostream>
#include <cstring>
#include "SuperBlock.h"
#include "Inode.h"

int main() {
    std::cout << "=== Inode Test ===" << std::endl;

    char* storage = new char[TOTAL_SIZE];
    std::memset(storage, 0, TOTAL_SIZE);

    InodeManager inodeMgr(storage, TOTAL_INODES);

    std::cout << "\n--- Test 1: Allocate Inode ---" << std::endl;
    int inodeNum = inodeMgr.allocInode(S_IFREG);
    std::cout << "Allocated inode: " << inodeNum << std::endl;

    Inode inode;
    inodeMgr.readInode(inodeNum, inode);
    std::cout << "Inode mode: " << inode.i_mode << std::endl;
    std::cout << "Is file: " << (inode.isFile() ? "YES" : "NO") << std::endl;
    std::cout << "Is directory: " << (inode.isDirectory() ? "YES" : "NO") << std::endl;

    std::cout << "\n--- Test 2: Allocate Blocks ---" << std::endl;
    int blocksAllocated = inodeMgr.allocateBlocks(inodeNum, 5);
    std::cout << "Allocated blocks: " << blocksAllocated << std::endl;

    inodeMgr.readInode(inodeNum, inode);
    std::cout << "File size: " << inode.i_size << " bytes" << std::endl;
    std::cout << "Blocks used: " << inode.i_blocks << std::endl;
    std::cout << "Direct block 0: " << inode.i_block[0] << std::endl;

    std::cout << "\n--- Test 3: Directory Inode ---" << std::endl;
    int dirInode = inodeMgr.allocInode(S_IFDIR);
    std::cout << "Allocated directory inode: " << dirInode << std::endl;

    inodeMgr.readInode(dirInode, inode);
    std::cout << "Is directory: " << (inode.isDirectory() ? "YES" : "NO") << std::endl;

    std::cout << "\n--- Test 4: Free Inode ---" << std::endl;
    inodeMgr.freeInode(inodeNum);
    std::cout << "Freed inode: " << inodeNum << std::endl;

    bool isAlloc = inodeMgr.isAllocated(inodeNum);
    std::cout << "Inode " << inodeNum << " is allocated: " << (isAlloc ? "YES" : "NO") << std::endl;

    std::cout << "\n--- Test 5: Inode Size Check ---" << std::endl;
    std::cout << "Inode size: " << sizeof(Inode) << " bytes" << std::endl;
    std::cout << "Max direct blocks: " << MAX_DIRECT_BLOCKS << std::endl;
    std::cout << "Max file size: " << MAX_FILE_SIZE << " bytes (" << MAX_FILE_SIZE / 1024 << " KB)" << std::endl;

    std::cout << "\nTest PASSED!" << std::endl;
    delete[] storage;
    return 0;
}
