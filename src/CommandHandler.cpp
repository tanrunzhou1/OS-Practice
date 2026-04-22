#include "CommandHandler.h"
#include <iostream>
#include <sstream>
#include <algorithm>

CommandHandler::CommandHandler(FileSystem* fs) : fileSystem_(fs), running_(false) {
}

CommandHandler::~CommandHandler() {
}

void CommandHandler::printHelp() {
    std::cout << "\n========== Available Commands ==========" << std::endl;
    std::cout << "  createFile <path> <sizeKB>  - Create a file" << std::endl;
    std::cout << "  rm <path>                  - Delete a file" << std::endl;
    std::cout << "  mkdir <path>               - Create a directory" << std::endl;
    std::cout << "  rmdir <path>              - Delete a directory" << std::endl;
    std::cout << "  cd <path>                  - Change directory" << std::endl;
    std::cout << "  dir                        - List directory contents" << std::endl;
    std::cout << "  ls                         - List directory contents" << std::endl;
    std::cout << "  cat <path>                 - Display file contents" << std::endl;
    std::cout << "  cp <source> <dest>         - Copy file" << std::endl;
    std::cout << "  sum                        - Show disk space info" << std::endl;
    std::cout << "  save <filename>            - Save filesystem to file" << std::endl;
    std::cout << "  load <filename>            - Load filesystem from file" << std::endl;
    std::cout << "  help                       - Show this help" << std::endl;
    std::cout << "  exit                       - Exit (auto save)" << std::endl;
    std::cout << "========================================" << std::endl;
}

void CommandHandler::printPrompt() {
    std::cout << fileSystem_->getCurrentDir() << " $ ";
}

std::vector<std::string> CommandHandler::parseCommandLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string CommandHandler::getCommandName(const std::vector<std::string>& tokens) {
    if (tokens.empty()) {
        return "";
    }
    return tokens[0];
}

std::vector<std::string> CommandHandler::getCommandArgs(const std::vector<std::string>& tokens) {
    if (tokens.size() <= 1) {
        return std::vector<std::string>();
    }
    return std::vector<std::string>(tokens.begin() + 1, tokens.end());
}

void CommandHandler::handleCreateFile(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "Usage: createFile <path> <sizeKB>" << std::endl;
        return;
    }
    std::string path = args[0];
    int sizeKB = std::stoi(args[1]);
    fileSystem_->createFile(path, sizeKB);
}

void CommandHandler::handleDeleteFile(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: rm <path>" << std::endl;
        return;
    }
    fileSystem_->deleteFile(args[0]);
}

void CommandHandler::handleCreateDir(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: mkdir <path>" << std::endl;
        return;
    }
    fileSystem_->createDir(args[0]);
}

void CommandHandler::handleDeleteDir(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: rmdir <path>" << std::endl;
        return;
    }
    fileSystem_->deleteDir(args[0]);
}

void CommandHandler::handleChangeDir(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: cd <path>" << std::endl;
        return;
    }
    fileSystem_->changeDir(args[0]);
}

void CommandHandler::handleListDir(const std::vector<std::string>& args) {
    (void)args;
    fileSystem_->dir();
}

void CommandHandler::handleReadFile(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: cat <path>" << std::endl;
        return;
    }
    std::string content = fileSystem_->readFile(args[0]);
    if (!content.empty()) {
        std::cout << content << std::endl;
    }
}

void CommandHandler::handleCopyFile(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "Usage: cp <source> <dest>" << std::endl;
        return;
    }
    fileSystem_->copyFile(args[0], args[1]);
}

void CommandHandler::handleSpaceInfo(const std::vector<std::string>& args) {
    (void)args;
    std::cout << fileSystem_->getSpaceInfo();
}

void CommandHandler::handleSave(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: save <filename>" << std::endl;
        return;
    }
    fileSystem_->save(args[0]);
}

void CommandHandler::handleLoad(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: load <filename>" << std::endl;
        return;
    }
    fileSystem_->load(args[0]);
}

void CommandHandler::run() {
    running_ = true;
    std::string line;
    
    fileSystem_->printWelcome();
    printHelp();
    
    while (running_) {
        printPrompt();
        
        if (!std::getline(std::cin, line)) {
            break;
        }
        
        std::transform(line.begin(), line.end(), line.begin(), ::tolower);
        
        std::vector<std::string> tokens = parseCommandLine(line);
        if (tokens.empty()) {
            continue;
        }
        
        std::string command = getCommandName(tokens);
        std::vector<std::string> args = getCommandArgs(tokens);
        
        if (command == "createfile" || command == "touch") {
            handleCreateFile(args);
        } else if (command == "rm" || command == "delete") {
            handleDeleteFile(args);
        } else if (command == "mkdir") {
            handleCreateDir(args);
        } else if (command == "rmdir") {
            handleDeleteDir(args);
        } else if (command == "cd") {
            handleChangeDir(args);
        } else if (command == "dir" || command == "ls") {
            handleListDir(args);
        } else if (command == "cat") {
            handleReadFile(args);
        } else if (command == "cp") {
            handleCopyFile(args);
        } else if (command == "sum") {
            handleSpaceInfo(args);
        } else if (command == "save") {
            handleSave(args);
        } else if (command == "load") {
            handleLoad(args);
        } else if (command == "help" || command == "?") {
            printHelp();
        } else if (command == "exit" || command == "quit" || command == "q") {
            std::cout << "Saving filesystem..." << std::endl;
            fileSystem_->save("data/fs.dat");
            std::cout << "Goodbye!" << std::endl;
            running_ = false;
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            std::cerr << "Type 'help' for available commands." << std::endl;
        }
    }
}

void CommandHandler::stop() {
    running_ = false;
}
