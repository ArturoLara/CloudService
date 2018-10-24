#ifndef TERMINAL_H
#define TERMINAL_H

#include <vector>

#include "datatree.h"

enum command_e
{
    NO_COMMAND = 0,
    CD = 1,
    LS = 2,
    PWD = 3,
    MV = 4,
    CP = 5,
    MKDIR = 6,
    RMDIR = 7,
    RM = 8,
    LLS = 9,
    LCD = 10,
    LPWD = 11,
    UPLOAD = 12,
    END = 13
};

typedef struct command_t{
    command_e type;
    std::vector<char*>* args;

    void clean()
    {
        type = command_e::NO_COMMAND;
        args->clear();
    }
}command_t;

class Terminal{

private:
    DataTree* tree;
    bool exit=false;
    bool findSubdirectoryDependency(node_t* OriginDirectory, node_t* destDirectory);
    std::string pwdRecursive(node_t* node);
    node_t* findByPathRecursive(char* command, node_t* actualDirectory, int index, bool dirFlag);
    node_t* findByPath(char* command);
    node_t* findDirectoryAtDirectory(node_t* directoryNode, std::string DirectoryName);
    node_t* findFileAtDirectory(node_t* directoryNode, std::string fileName);
    node_t* findNodeAtDirectory(node_t* directoryNode, std::string nodeName);
    void copyDirectoryRecursive(node_t* OriginDirectory, node_t* destDirectory, std::string newNameDirectory);
    void copyLocalDirectoryRecursive(node_t* destDirectory);
    std::string getOutFromCommand(std::string cmd);

public:
    Terminal();
    void run();
    void readCommand(command_t* comando);
    void runCommand(command_t comando);
    command_e getTypeOfCommand(char* comando);
    void ls();
    void cd(command_t command);
    void pwd();
    void mkdir(command_t command);
    void rmdir(command_t command);
    void rm(command_t aCommand);
    void upload(command_t aCommand);
    void mv(command_t aCommand);
    void cp(command_t aCommand);
    void lls();
    void lcd(command_t aCommand);
    void lpwd();
};

#endif // TERMINAL_H
