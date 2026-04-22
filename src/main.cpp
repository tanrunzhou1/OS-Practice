#include <iostream>
#include "FileSystem.h"
#include "CommandHandler.h"

int main() {
    FileSystem* fs = new FileSystem();
    CommandHandler* handler = new CommandHandler(fs);
    
    handler->run();
    
    delete handler;
    delete fs;
    
    return 0;
}
