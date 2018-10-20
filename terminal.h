#ifndef TERMINAL_H
#define TERMINAL_H

#include <vector>

#include "datatree.h"

enum command_e
{
    noCommand,
    cd,
    ls,
    pwd,
    mv,
    cp,
    mkdir,
    rmdir,
    rm,
    lls,
    lcd,
    lpwd,
    upload,
    end
};

typedef struct command_t{
    command_e type;
    std::vector<char*>* args;
}command_t;

class Terminal{

private:

    std::string pwdRecursive(node_t* node);


public:

    DataTree* tree;
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
