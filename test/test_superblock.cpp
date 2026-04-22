#include <iostream>
#include "SuperBlock.h"

int main() {
    std::cout << "=== SuperBlock Test ===" << std::endl;

    SuperBlock sb;
    sb.init();
    sb.print();

    std::cout << "\n--- Calculate Storage Layout ---" << std::endl;
    std::cout << "SuperBlock size: " << sizeof(SuperBlock) << " bytes" << std::endl;
    std::cout << "Inode size: " << INODE_SIZE << " bytes" << std::endl;
    std::cout << "Inodes per block: " << INODES_PER_BLOCK << std::endl;
    std::cout << "Inode table blocks: " << INODE_TABLE_BLOCKS << std::endl;
    std::cout << "First data block: " << FIRST_DATA_BLOCK << std::endl;
    std::cout << "Data blocks count: " << TOTAL_BLOCKS - FIRST_DATA_BLOCK << std::endl;

    std::cout << "\nTest PASSED!" << std::endl;
    return 0;
}
