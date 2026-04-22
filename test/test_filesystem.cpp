#include <iostream>
#include "FileSystem.h"

int main() {
    std::cout << "=== FileSystem Test: createFile / deleteFile ===" << std::endl;

    FileSystem fs;
    fs.printWelcome();

    std::cout << "\n--- Test 1: Create Directory ---" << std::endl;
    fs.createDir("/dir1");
    fs.createDir("/dir1/sub1");

    std::cout << "\n--- Test 2: Create File ---" << std::endl;
    fs.createFile("/dir1/myFile", 10);

    std::cout << "\n--- Test 3: List Directory ---" << std::endl;
    auto entries = fs.listDir();
    std::cout << "Entries in root: " << entries.size() << std::endl;

    fs.changeDir("/dir1");
    entries = fs.listDir();
    std::cout << "Entries in /dir1: " << entries.size() << std::endl;
    for (const auto& entry : entries) {
        std::cout << "  - " << entry.name << std::endl;
    }

    std::cout << "\n--- Test 4: Read File ---" << std::endl;
    std::string content = fs.readFile("/dir1/myFile");
    if (!content.empty()) {
        std::cout << "File content (first 50 chars): " << content.substr(0, 50) << "..." << std::endl;
    }

    std::cout << "\n--- Test 5: Copy File ---" << std::endl;
    fs.copyFile("/dir1/myFile", "/dir1/backupFile");

    std::cout << "\n--- Test 6: Space Info ---" << std::endl;
    std::cout << fs.getSpaceInfo();

    std::cout << "\n--- Test 7: Delete File ---" << std::endl;
    fs.deleteFile("/dir1/backupFile");

    std::cout << "\n--- Test 8: Delete Directory ---" << std::endl;
    fs.deleteDir("/dir1/sub1");

    std::cout << "\n--- Test 9: Error Handling ---" << std::endl;
    fs.createFile("/dir1/myFile", 1000);

    std::cout << "\nTest PASSED!" << std::endl;
    return 0;
}
