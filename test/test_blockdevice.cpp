#include <iostream>
#include "BlockDevice.h"

int main() {
    std::cout << "=== BlockDevice Test ===" << std::endl;

    BlockDevice device;

    std::cout << "Block Size: " << device.getBlockSize() << " bytes" << std::endl;
    std::cout << "Total Blocks: " << device.getTotalBlocks() << std::endl;
    std::cout << "Total Size: " << device.getTotalSize() << " bytes (" 
              << device.getTotalSize() / 1024 / 1024 << " MB)" << std::endl;

    char writeBuffer[1024];
    char readBuffer[1024];

    for (int i = 0; i < 1024; i++) {
        writeBuffer[i] = 'A' + (i % 26);
    }

    std::cout << "\nWriting to block 0..." << std::endl;
    device.write(0, writeBuffer);

    std::cout << "Reading from block 0..." << std::endl;
    device.read(0, readBuffer);

    bool success = true;
    for (int i = 0; i < 100; i++) {
        if (writeBuffer[i] != readBuffer[i]) {
            success = false;
            break;
        }
    }

    if (success) {
        std::cout << "Test PASSED: Write and Read successful!" << std::endl;
    } else {
        std::cout << "Test FAILED: Data mismatch!" << std::endl;
    }

    std::cout << "\nFirst 50 bytes read: ";
    for (int i = 0; i < 50; i++) {
        std::cout << readBuffer[i];
    }
    std::cout << std::endl;

    return 0;
}
