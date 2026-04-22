#include <iostream>
#include "Bitmap.h"

int main() {
    std::cout << "=== Bitmap Test ===" << std::endl;

    Bitmap bitmap(100);

    std::cout << "Max Size: " << bitmap.getMaxSize() << std::endl;
    std::cout << "Initial Free: " << bitmap.getFreeCount() << std::endl;

    int index1 = bitmap.allocate();
    std::cout << "Allocated index: " << index1 << std::endl;
    std::cout << "Free count after allocate: " << bitmap.getFreeCount() << std::endl;

    bool isAlloc = bitmap.isAllocated(index1);
    std::cout << "Index " << index1 << " is allocated: " << (isAlloc ? "YES" : "NO") << std::endl;

    int index2 = bitmap.allocate();
    int index3 = bitmap.allocate();
    std::cout << "Allocated more: " << index2 << ", " << index3 << std::endl;
    std::cout << "Free count: " << bitmap.getFreeCount() << std::endl;

    bitmap.release(index2);
    std::cout << "Released index: " << index2 << std::endl;
    std::cout << "Free count after release: " << bitmap.getFreeCount() << std::endl;

    bool isAlloc2 = bitmap.isAllocated(index2);
    std::cout << "Index " << index2 << " is allocated: " << (isAlloc2 ? "YES" : "NO") << std::endl;

    std::cout << "\nTest PASSED!" << std::endl;

    return 0;
}
