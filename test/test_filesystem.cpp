#include <iostream>
#include "FileSystem.h"

int main() {
    std::cout << "=== FileSystem Test: Directory Operations ===" << std::endl;

    FileSystem fs;
    fs.printWelcome();

    std::cout << "\n--- Test 1: Create Directories ---" << std::endl;
    fs.createDir("/dir1");

    std::cout << "\n--- Test 2: List Root Directory ---" << std::endl;
    fs.dir();

    std::cout << "\n--- Test 3: Change to dir1 ---" << std::endl;
    fs.changeDir("/dir1");

    std::cout << "\n--- Test 4: List dir1 contents ---" << std::endl;
    fs.dir();

    std::cout << "\n--- Test 5: Create file in dir1 ---" << std::endl;
    fs.createFile("/dir1/test.txt", 1);

    std::cout << "\n--- Test 6: List dir1 again ---" << std::endl;
    fs.dir();

    std::cout << "\n--- Test 7: Go to root ---" << std::endl;
    fs.changeDir("/");
    fs.dir();

    std::cout << "\n--- Test 8: Space Info ---" << std::endl;
    std::cout << fs.getSpaceInfo();

    std::cout << "\nTest PASSED!" << std::endl;
    return 0;
}
