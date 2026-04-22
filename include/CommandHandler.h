#pragma once

#include <string>
#include <vector>
#include "FileSystem.h"

class CommandHandler {
private:
    FileSystem* fileSystem_;
    bool running_;

    void printHelp();
    void printPrompt();
    std::vector<std::string> parseCommandLine(const std::string& line);
    std::string getCommandName(const std::vector<std::string>& tokens);
    std::vector<std::string> getCommandArgs(const std::vector<std::string>& tokens);

    void handleCreateFile(const std::vector<std::string>& args);
    void handleDeleteFile(const std::vector<std::string>& args);
    void handleCreateDir(const std::vector<std::string>& args);
    void handleDeleteDir(const std::vector<std::string>& args);
    void handleChangeDir(const std::vector<std::string>& args);
    void handleListDir(const std::vector<std::string>& args);
    void handleReadFile(const std::vector<std::string>& args);
    void handleCopyFile(const std::vector<std::string>& args);
    void handleSpaceInfo(const std::vector<std::string>& args);
    void handleSave(const std::vector<std::string>& args);
    void handleLoad(const std::vector<std::string>& args);

public:
    CommandHandler(FileSystem* fs);
    ~CommandHandler();

    void run();
    void stop();
};
