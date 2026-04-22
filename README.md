# Unix风格文件系统模拟器

---

## 项目简介

本项目是一个基于i-node的Unix风格虚拟文件系统模拟器，旨在内存中模拟真实的Unix文件系统架构。

通过本项目，你可以学习到：

- i-node的工作原理
- 数据块的分配与管理
- 目录的实现方式
- 文件系统超级块的作用
- 位图管理机制

---

## 如何运行

### 环境要求

- Windows 10/11 或 macOS/Linux
- C++ 编译器 (g++ 或 Visual Studio)

### 编译

```bash
# 进入项目目录
cd d:\scut\Grade3\OS Practice

# 创建 data 目录（如果不存在）
mkdir data

# 编译（Windows PowerShell）
g++ -std=c++11 -o filesystem.exe src/main.cpp src/FileSystem.cpp src/Directory.cpp src/Inode.cpp src/SuperBlock.cpp src/Bitmap.cpp src/BlockDevice.cpp src/CommandHandler.cpp -Iinclude
```

### 运行

```bash
# 运行程序
.\filesystem.exe
```

### 退出

输入 `exit` 命令退出系统，会自动保存文件系统到 `data/fs.dat`。

---

## 命令列表

| 命令 | 用法 | 说明 |
|------|------|------|
| **mkdir** | `mkdir <路径>` | 创建目录 |
| **rmdir** | `rmdir <路径>` | 删除目录 |
| **cd** | `cd <路径>` | 切换目录 |
| **dir** | `dir` | 列出目录内容 |
| **ls** | `ls` | 列出目录内容 |
| **createFile** | `createFile <路径> <大小KB>` | 创建文件 |
| **rm** | `rm <路径>` | 删除文件 |
| **cat** | `cat <路径>` | 查看文件内容 |
| **cp** | `cp <源文件> <目标文件>` | 复制文件 |
| **sum** | `sum` | 显示磁盘空间信息 |
| **save** | `save <文件名>` | 保存文件系统 |
| **load** | `load <文件名>` | 加载文件系统 |
| **help** | `help` | 显示帮助 |
| **exit** | `exit` | 退出（自动保存） |

---

## 使用示例

```
========== Available Commands ==========
  createFile <path> <sizeKB>  - Create a file
  rm <path>                  - Delete a file
  mkdir <path>               - Create a directory
  rmdir <path>              - Delete a directory
  cd <path>                 - Change directory
  dir                        - List directory contents
  ls                         - List directory contents
  cat <path>                 - Display file contents
  cp <source> <dest>         - Copy file
  sum                        - Show disk space info
  save <filename>            - Save filesystem to file
  load <filename>           - Load filesystem from file
  help                       - Show this help
  exit                       - Exit (auto save)
========================================

/ $ mkdir /home
Directory created: /home

/ $ cd /home
Changed directory to: /home

/home $ createFile test.txt 5
File created: /test.txt (5 KB)

/home $ dir
Type      Size        Modified Time       Name
------------------------------------------------------------
<FILE>    5120 B      2026-04-22 17:30:00 test.txt
------------------------------------------------------------
  Total: 1 items

/home $ cat test.txt
[文件内容显示]

/home $ sum
========== Storage Space Info ==========
Total Blocks:      16384
Used Blocks:       10
Free Blocks:       16374
Total Inodes:      1000
Used Inodes:       2
Free Inodes:       998
=========================================

/home $ exit
Saving filesystem...
Goodbye!
```

---

## 技术栈

| 技术 | 说明 |
|------|------|
| **C++11/14** | 编程语言，仅使用标准库 |
| **BlockDevice** | 16MB内存虚拟磁盘 |
| **Bitmap** | i-node和数据块位图管理 |
| **Inode** | 文件元信息存储结构 |
| **Directory** | 目录项管理 |
| **CommandHandler** | 命令解析与分发 |
| **fstream** | 文件持久化 |

---

## 功能列表

- [x] 欢迎信息显示
- [x] 创建/删除文件
- [x] 创建/删除目录
- [x] 切换工作目录
- [x] 列出目录内容（含属性）
- [x] 复制文件
- [x] 查看文件内容
- [x] 查看空间使用
- [x] 持久化存储

---

## 项目结构

```
UnixFileSystem/
├── include/                  # 头文件
│   ├── BlockDevice.h
│   ├── SuperBlock.h
│   ├── Inode.h
│   ├── Bitmap.h
│   ├── Directory.h
│   ├── FileSystem.h
│   └── CommandHandler.h
├── src/                     # 源代码
│   ├── main.cpp
│   ├── BlockDevice.cpp
│   ├── SuperBlock.cpp
│   ├── Inode.cpp
│   ├── Bitmap.cpp
│   ├── Directory.cpp
│   ├── FileSystem.cpp
│   └── CommandHandler.cpp
├── data/                    # 运行时数据
│   └── fs.dat              # 文件系统镜像
└── README.md                # 项目说明
```

详细说明请查看 [ARCHITECTURE.md](./docs/ARCHITECTURE.md)

---



*学习项目，仅供交流参考*
