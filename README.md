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

### 编译运行

#### 方式一：使用 g++ (MinGW)

```bash
# 1. 编译
g++ -std=c++11 -o filesystem main.cpp

# 2. 运行
./filesystem
```

#### 方式二：使用 Visual Studio

1. 创建C++控制台项目
2. 添加源文件到项目
3. 编译运行 (Ctrl + F5)

### 操作示例

```
# 创建目录
createDir /dir1

# 创建文件 (10KB)
createFile /dir1/myFile 10

# 查看目录
dir

# 切换目录
changeDir /dir1

# 查看文件内容
cat /dir1/myFile

# 复制文件
cp myFile backupFile

# 查看空间使用
sum

# 退出 (自动保存)
exit
```

---

## 技术栈

| 技术 | 说明 |
|------|------|
| **C++11/14** | 编程语言，仅使用标准库 |
| **BlockDevice** | 16MB内存虚拟磁盘 |
| **Bitmap** | i-node和数据块位图管理 |
| **Inode** | 文件元信息存储结构 |
| **fstream** | 文件持久化 |

---

## 功能列表

- [x] 欢迎信息显示
- [x] 创建/删除文件
- [x] 创建/删除目录
- [x] 切换工作目录
- [x] 列出目录内容
- [x] 复制文件
- [x] 查看文件内容
- [x] 查看空间使用
- [x] 持久化存储

---

## 项目结构

```
UnixFileSystem/
├── include/          # 头文件
├── src/              # 源代码
├── data/             # 运行时数据
├── Makefile          # 构建配置
└── README.md         # 项目说明
```

详细说明请查看 [ARCHITECTURE.md](./ARCHITECTURE.md)

---



*学习项目，仅供交流参考*
