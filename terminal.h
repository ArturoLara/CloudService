#ifndef TERMINAL_H
#define TERMINAL_H

#include <vector>

#include "datatree.h"

const int CMD_CD = 1;
const int CMD_MKDIR = 2;
const int CMD_EXIT = 3;

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
    exit
};

typedef struct command_t{
    command_e type;
    std::vector<char*> args;
}command_t;

class Terminal{

private:

    std::string pwdRecursive(node_t node);


public:

    DataTree* tree;
    Terminal();
    void run();
    void lee_comando(comando_t* comando);
    void ejecuta_comando(comando_t comando);
    int get_tipo_comando(char* comando);
    void ls();
    void cd(command_t command);
    void pwd();
    void mkdir(command_t command);
};

#endif // TERMINAL_H
