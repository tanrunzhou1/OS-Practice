# UnixFileSystem 架构设计文档

---

## 一、技术栈选择

### 1.1 编程语言

**C++11/14**

- 项目要求使用C++
- 能够直接操作内存，适合理解文件系统底层机制
- C++11的智能指针、自动类型推导等特性提升开发效率

### 1.2 开发环境

| 工具 | 选择 | 理由 |
|------|------|------|
| 编译器 | g++ (MinGW) / Visual Studio | 主流C++编译器 |
| 编辑器 | VS Code | 轻量、跨平台、智能提示 |
| 调试器 | GDB | 支持断点、单步调试 |

### 1.3 只使用标准库

| 头文件 | 用途 |
|--------|------|
| `<iostream>` | 输入输出 |
| `<fstream>` | 文件读写（持久化） |
| `<string>` | 字符串处理 |
| `<vector>` | 动态数组 |
| `<unordered_map>` | 哈希表（快速查找） |
| `<cstdint>` | 固定宽度整数 |
| `<ctime>` | 时间处理 |
| `<cstdlib>` | 随机字符串生成 |
| `<memory>` | 智能指针 |

---

## 二、项目目录结构

```
UnixFileSystem/
├── include/                  # 头文件目录
│   ├── BlockDevice.h        # 块设备抽象层
│   ├── SuperBlock.h         # 超级块定义
│   ├── Inode.h              # i-node定义
│   ├── Bitmap.h             # 位图管理
│   ├── Directory.h          # 目录结构定义
│   ├── FileSystem.h         # 文件系统核心
│   └── CommandHandler.h     # 命令处理
├── src/                     # 源代码目录
│   ├── main.cpp             # 主程序入口
│   ├── BlockDevice.cpp
│   ├── SuperBlock.cpp
│   ├── Inode.cpp
│   ├── Bitmap.cpp
│   ├── Directory.cpp
│   ├── FileSystem.cpp
│   └── CommandHandler.cpp
├── data/                    # 数据文件目录（运行时生成）
│   └── fs.dat               # 文件系统镜像
├── docs/                    # 文档目录
│   ├── PRD.md               # 产品需求文档
│   └── ARCHITECTURE.md      # 本文档
├── Makefile                 # 构建配置
└── README.md                # 项目说明
```

---

## 三、核心模块说明

### 3.1 模块架构图

```
┌─────────────────────────────────────────────────────────────────┐
│                        main.cpp                                  │
│                    (程序入口 & 主循环)                            │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    CommandHandler                               │
│                  (命令解析 & 分发)                                │
│  - parseCommand()                                               │
│  - dispatch()                                                   │
└─────────────────────────────────────────────────────────────────┘
            │           │           │           │
            ▼           ▼           ▼           ▼
┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
│ createFile  │ │ deleteFile  │ │  createDir  │ │  changeDir  │
│    cat      │ │    cp       │ │  deleteDir  │ │    dir      │
│    sum      │ │   exit      │ │             │ │             │
└─────────────┘ └─────────────┘ └─────────────┘ └─────────────┘
            │           │           │
            ▼           ▼           ▼
┌─────────────────────────────────────────────────────────────────┐
│                      FileSystem                                  │
│                   (文件系统核心引擎)                              │
├─────────────────────────────────────────────────────────────────┤
│ + createFile()   + deleteFile()   + createDir()                │
│ + deleteDir()    + changeDir()    + listDir()                 │
│ + copyFile()     + readFile()     + getSpaceInfo()            │
│ + save()         + load()                                        │
└─────────────────────────────────────────────────────────────────┘
            │           │           │           │
            ▼           ▼           ▼           ▼
┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
│  InodeMgr   │ │  BlockMgr   │ │  BitmapMgr  │ │  DirMgr     │
│  (i-node)   │ │  (数据块)    │ │  (位图)      │ │  (目录)      │
└─────────────┘ └─────────────┘ └─────────────┘ └─────────────┘
            │           │           │
            ▼           ▼           ▼
┌─────────────────────────────────────────────────────────────────┐
│                     BlockDevice                                 │
│                  (块设备抽象层 - 16MB)                           │
│  - 内存分配                                                      │
│  - 块读写                                                        │
│  - 持久化                                                        │
└─────────────────────────────────────────────────────────────────┘
```

### 3.2 各模块职责

| 模块 | 职责 | 关键接口 |
|------|------|---------|
| **CommandHandler** | 解析用户输入，分发到对应功能 | `parseCommand()`, `dispatch()` |
| **FileSystem** | 对外提供文件系统API | `createFile()`, `deleteFile()`, `createDir()` |
| **InodeMgr** | i-node分配、释放、读写 | `allocInode()`, `freeInode()`, `readInode()` |
| **BlockMgr** | 数据块分配、释放、读写 | `allocBlock()`, `freeBlock()`, `readBlock()` |
| **BitmapMgr** | 位图管理 | `setBit()`, `clearBit()`, `findFree()` |
| **DirMgr** | 目录项管理 | `addEntry()`, `removeEntry()`, `listEntries()` |
| **BlockDevice** | 底层存储抽象 | `read()`, `write()`, `sync()` |

---

## 四、数据模型设计

### 4.1 块设备 (BlockDevice)

```cpp
class BlockDevice {
private:
    static const int BLOCK_SIZE = 1024;      // 1KB
    static const int TOTAL_BLOCKS = 16384;   // 16MB / 1KB
    static const int TOTAL_SIZE = 16777216; // 16MB
    
    char* storage;                            // 16MB内存
    
public:
    BlockDevice();
    ~BlockDevice();
    
    void read(int blockNum, char* buffer);
    void write(int blockNum, const char* buffer);
    void syncToDisk(const std::string& filename);
    void loadFromDisk(const std::string& filename);
};
```

### 4.2 超级块 (SuperBlock)

```cpp
struct SuperBlock {
    uint32_t s_inodes_count;      // i-node总数 (1000)
    uint32_t s_blocks_count;      // 块总数 (16384)
    uint32_t s_free_blocks_count; // 空闲块数
    uint32_t s_free_inodes_count; // 空闲i-node数
    uint32_t s_first_data_block;  // 第一个数据块号
    uint32_t s_block_bitmap_block; // 块位图所在块
    uint32_t s_inode_bitmap_block; // i-node位图所在块
    uint32_t s_inode_table_block; // i-node表起始块
};
```

### 4.3 i-node (Inode)

```cpp
struct Inode {
    uint16_t i_mode;               // 文件类型和权限 (2字节)
    uint16_t i_uid;                // 用户ID (2字节)
    uint32_t i_size;               // 文件大小（字节）(4字节)
    uint32_t i_atime;              // 最后访问时间 (4字节)
    uint32_t i_ctime;              // 创建时间 (4字节)
    uint32_t i_mtime;              // 修改时间 (4字节)
    uint16_t i_links_count;        // 硬链接数 (2字节)
    uint16_t i_blocks;             // 占用的块数 (2字节)
    uint32_t i_block[10];          // 10个直接块地址 (40字节)
    uint32_t i_indirect_block;      // 一级间接块地址 (4字节)
    
    // 总计: 64字节
};
```

**最大文件大小计算**：

- 直接块：10 × 1KB = 10KB
- 间接块：256 × 1KB = 256KB（一级间接可存256个块地址）
- 总计：266KB

### 4.4 目录项 (Directory Entry)

```cpp
struct DirEntry {
    uint32_t inode_num;    // i-node号 (4字节)
    char name[28];         // 文件名 (28字节)
    
    // 总计: 32字节
};
```

### 4.5 磁盘布局

```
┌─────────────────────────────────────────────────────────────────┐
│                         16MB 虚拟磁盘                           │
├─────┬─────┬─────┬─────┬────────────────────────────────────────┤
│  0  │  1  │  2  │ 3~18│ 19~16383                                │
│超级块│块位图│i-node│i-node│ 数据块                                 │
│     │     │位图  │表    │                                        │
│1KB  │1KB  │1KB  │16KB  │ ~16MB - 19KB                           │
└─────┴─────┴─────┴─────┴────────────────────────────────────────┘

详细分配：
- Block 0:   SuperBlock (1KB)
- Block 1:   块位图 Block Bitmap (1KB, 管理16384个块)
- Block 2:   i-node位图 Inode Bitmap (1KB, 管理1000个i-node)
- Block 3:   块位图备份 (1KB)
- Block 4:   i-node位图备份 (1KB)
- Block 5-18: i-node表 (14KB = 14块 × 1KB, 可存224个i-node)
- Block 19+: 数据块区域
```

---

## 五、代码规范

### 5.1 命名规范

| 类型 | 规范 | 示例 |
|------|------|------|
| 类名 | 大驼峰 | `class BlockDevice` |
| 函数名 | 小驼峰 | `void readBlock()` |
| 成员变量 | 下划线法 | `int block_num_` |
| 常量 | 全大写下划线 | `const int BLOCK_SIZE = 1024` |
| 头文件 | 与类名相同 | `BlockDevice.h` |

### 5.2 文件头注释

```cpp
/**
 * @file Inode.h
 * @brief i-node数据结构定义
 * @author 你的名字
 * @date 2026-04-22
 */
```

### 5.3 函数注释

```cpp
/**
 * @brief 分配一个空闲i-node
 * @return 成功返回i-node编号，失败返回-1
 * @note 会自动初始化i-node结构
 */
int allocInode();
```

### 5.4 错误处理

```cpp
enum class ErrorCode {
    SUCCESS = 0,
    ENOENT = 1,      // 文件不存在
    ENOSPC = 2,      // 空间不足
    EEXIST = 3,      // 文件已存在
    EINVAL = 4,      // 无效参数
    EBUSY = 5        // 资源忙
};
```

### 5.5 代码组织原则

1. 头文件只放声明，不放实现（除非是inline）
2. 每个类一个头文件+一个cpp文件
3. 公共接口放头文件，私有实现放cpp
4. 使用 `#pragma once` 防止重复包含

---

## 六、实现顺序

```
第1周：基础设施
├── BlockDevice (内存分配/读写)
├── BitmapMgr (位图管理)
└── SuperBlock (超级块初始化)

第2周：i-node核心
├── Inode结构定义
├── InodeMgr (分配/释放/读写)
└── 根目录创建

第3周：文件操作
├── createFile / deleteFile
├── cat (读取文件内容)
└── cp (复制文件)

第4周：目录操作
├── createDir / deleteDir
├── changeDir
└── dir (列出目录)

第5周：系统功能
├── sum (空间统计)
├── 欢迎信息
└── 持久化 (exit/load)

第6周：测试与优化
├── 边界测试
├── 性能优化
└── 代码整理
```

---

## 七、关键技术点

### 7.1 位图管理

使用位图跟踪空闲资源：

```cpp
// 块位图：16384位 = 2KB
// i-node位图：1000位 = 125字节
```

### 7.2 路径解析

支持绝对路径和相对路径：

- 绝对路径：`/dir1/sub1/file.txt`
- 相对路径：`dir1/file.txt`（相对于当前目录）

### 7.3 持久化

使用二进制格式保存整个16MB虚拟磁盘到文件：

```cpp
void save(const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    out.write(storage_, TOTAL_SIZE);
}
```

---

*文档版本：v1.0*
*创建日期：2026-04-22*
