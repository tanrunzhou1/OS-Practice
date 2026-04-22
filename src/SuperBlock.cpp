#include "SuperBlock.h"
#include <iostream>
#include <cstring>

void SuperBlock::init() {
    s_inodes_count = TOTAL_INODES;
    s_blocks_count = TOTAL_BLOCKS;
    s_free_blocks_count = TOTAL_BLOCKS - FIRST_DATA_BLOCK;
    s_free_inodes_count = TOTAL_INODES;
    s_first_data_block = FIRST_DATA_BLOCK;
    s_block_bitmap_block = BLOCK_BITMAP_BLOCK;
    s_inode_bitmap_block = INODE_BITMAP_BLOCK;
    s_inode_table_block = INODE_TABLE_BLOCK;
}

void SuperBlock::print() const {
    std::cout << "========== SuperBlock Info ==========" << std::endl;
    std::cout << "Total Blocks:       " << s_blocks_count << std::endl;
    std::cout << "Total Inodes:       " << s_inodes_count << std::endl;
    std::cout << "Free Blocks:        " << s_free_blocks_count << std::endl;
    std::cout << "Free Inodes:        " << s_free_inodes_count << std::endl;
    std::cout << "First Data Block:   " << s_first_data_block << std::endl;
    std::cout << "Block Bitmap Block: " << s_block_bitmap_block << std::endl;
    std::cout << "Inode Bitmap Block: " << s_inode_bitmap_block << std::endl;
    std::cout << "Inode Table Block:  " << s_inode_table_block << std::endl;
    std::cout << "======================================" << std::endl;
}
