#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>
#include <cstring>

Terminal::Terminal()
{
    tree = new DataTree();
}

void Terminal::cd(command_t command){

    if(strncmp("..", command.args[0], 2))
    {
        node_t* node = this->tree->getActualDirectoryNode()->fatherNode;
        this->tree->setActualDirectoryNode(node);
    }
    else if(strncmp("/", command.args[0], 1))
    {
        this->tree->setActualDirectoryNode(tree->findNode(0));
    }
    else
    {
        for(node_t* node : this->tree->getActualDirectoryNode()->childNodes)
        {
            if(strncmp(command.args[0] , node->nameNode.c_str(), node->nameNode.size()))
            {
                this->tree->setActualDirectoryNode(node);
                break;
            }
        }
    }
}

void Terminal::ls(){
    std::cout << std::endl;
    for(node_t* node : this->tree->getActualDirectoryNode()->childNodes)
    {
        std::cout << node->nameNode << " ";
        if(node->directoryFlag)
        {
            std::cout << "(directory) ";
        }
        else
        {
            std::cout << "(file) ";
        }
        std::cout << node->size << "Bytes " << node->lastChange << std::endl;
    }
    std::endl;
}

std::string Terminal::pwdRecursive(node_t node){

    std::string directory;
    std::string name = node.nameNode;
    if(node.fatherNode != NULL)
    {
        directory = pwdRecursive(node.fatherNode);
    }
    directory += "/";
    directory += name;
    return directory;

}

void Terminal::pwd(){
    std::cout << std::endl << pwdRecursive(this->tree->getActualDirectoryNode()) << std::endl;
}

/*
void Terminal::mkdir(command_t command){
    command.args[1].
}
*/
void Terminal::run(){
    bool exit=false;
    command_t command;
    command.args= new vector<char*>();
    while(!exit){
        std::cout << "$: ";
        lee_comando(&command);
        ejecuta_comando(&command);
    }
}
void Terminal::lee_comando(command_t* command){
    char* line=new char [1024];
    char spacer[2]=" ";
    char* token=NULL;

    fgets(line, 1023, stdin);
    token=strtok(line, spacer);
    command->type=get_tipo_comando(token);

    while(token!=NULL){
        token=strtok(NULL, spacer);
        command->args->push_back(token);
    }
}

command_e Terminal::get_tipo_comando(char* commandArray)
{
    if(commandArray != NULL)
    {
        if(strncmp("cd", commandArray, 2))
            return command_e::cd;
        else if(strncmp("ls", commandArray, 2))
            return command_e::ls;
        else if(strncmp("pwd", commandArray, 3))
            return command_e::pwd;
        else if(strncmp("mv", commandArray, 2))
            return command_e::mv;
        else if(strncmp("cp", commandArray, 2))
            return command_e::cp;
        else if(strncmp("mkdir", commandArray, 5))
            return command_e::mkdir;
        else if(strncmp("rmdir", commandArray, 5))
            return command_e::rmdir;
        else if(strncmp("rm", commandArray, 2))
            return command_e::rm;
        else if(strncmp("lls", commandArray, 3))
            return command_e::lls;
        else if(strncmp("lcd", commandArray, 3))
            return command_e::lcd;
        else if(strncmp("lpwd", commandArray, 4))
            return command_e::lpwd;
        else if(strncmp("upload", commandArray, 6))
            return command_e::upload;
        else if(strncmp("exit", commandArray, 4))
            return command_e::exit;

    }
    return command_e::noCommand;
}
void Terminal::ejecuta_comando(command_t* command)
{
    switch(command->type)
    {
        case command_e::cd:
            break;
        case command_e::ls:
            break;
        case command_e::pwd:
            break;
        case command_e::mv:
            break;
        case command_e::cp:
            break;
        case command_e::mkdir:
            break;
        case command_e::rmdir:
            break;
        case command_e::rm:
            break;
        case command_e::lls:
            break;
        case command_e::lcd:
            break;
        case command_e::lpwd:
            break;
        case command_e::upload:
            break;
        case command_e::exit:
            break;
        case command_e::noCommand:
            std::cout << "Command not found" << std::endl;
            break;
        default:
            std::cout << "Command not found" << std::endl;
            break;
    }
}

//MAIN//////////////////////////////////

int main(){
    Terminal* term=new Terminal();
    term->run();
    return 0;
}
