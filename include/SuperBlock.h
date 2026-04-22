#pragma once

#include <cstdint>

const int BLOCK_SIZE = 1024;
const int TOTAL_BLOCKS = 16384;
const int TOTAL_SIZE = TOTAL_BLOCKS * BLOCK_SIZE;
const int TOTAL_INODES = 1000;
const int INODE_SIZE = 64;
const int INODES_PER_BLOCK = BLOCK_SIZE / INODE_SIZE;
const int INODE_TABLE_BLOCKS = (TOTAL_INODES + INODES_PER_BLOCK - 1) / INODES_PER_BLOCK;

const int SUPERBLOCK_BLOCK = 0;
const int BLOCK_BITMAP_BLOCK = 1;
const int INODE_BITMAP_BLOCK = 2;
const int INODE_TABLE_BLOCK = 5;
const int FIRST_DATA_BLOCK = INODE_TABLE_BLOCK + INODE_TABLE_BLOCKS;

struct SuperBlock {
    uint32_t s_inodes_count;
    uint32_t s_blocks_count;
    uint32_t s_free_blocks_count;
    uint32_t s_free_inodes_count;
    uint32_t s_first_data_block;
    uint32_t s_block_bitmap_block;
    uint32_t s_inode_bitmap_block;
    uint32_t s_inode_table_block;

    void init();
    void print() const;
};
