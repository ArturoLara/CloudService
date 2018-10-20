#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>

Terminal::Terminal()
{
    tree = new DataTree();
}

void Terminal::cd(command_t aCommand){

    bool completeFlag = false;
    if(strncmp("..", aCommand.args->at(0), 2))
    {
        node_t* node = this->tree->getActualDirectoryNode()->fatherNode;
        this->tree->setActualDirectoryNode(node);
        completeFlag = true;
    }
    else if(strncmp("/", aCommand.args->at(0), 1))
    {
        tree->setActualDirectoryNode(tree->findNode(0));
        completeFlag = true;
    }
    else
    {
        for(node_t* node : this->tree->getActualDirectoryNode()->childNodes)
        {
            if(strncmp(aCommand.args->at(0) , node->nameNode.c_str(), node->nameNode.size()))
            {
                if(node->directoryFlag)
                {
                    this->tree->setActualDirectoryNode(node);
                    completeFlag = true;
                    break;
                }
            }
        }
        if(!completeFlag)
        {
            std::cout << "It's not a directory" << std::endl;
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
    std::cout << std::endl;
}

std::string Terminal::pwdRecursive(node_t* aNode){

    std::string directory;
    std::string name = aNode->nameNode;
    if(aNode->fatherNode != NULL)
    {
        directory = pwdRecursive(aNode->fatherNode);
    }
    directory += "/";
    directory += name;
    return directory;

}

void Terminal::pwd(){
    std::cout << std::endl << pwdRecursive(this->tree->getActualDirectoryNode()) << std::endl;
}


void Terminal::mkdir(command_t aCommand){

    std::string nameOfDir(aCommand.args->at(0));
    tree->addNode(tree->getActualDirectoryNode(), nameOfDir, true, SIZE_OF_DIRECTORY);
}

void Terminal::rmdir(command_t aCommand){
    std::string dirToRemove(aCommand.args->at(0));
    node_t* nodeToRemove = NULL;
    int index;

    for(index = 0; index < nodeToRemove->fatherNode->childNodes.size(); index++)
    {
        node_t* node = nodeToRemove->fatherNode->childNodes.at(index);
        if(dirToRemove == node->nameNode)
        {
            nodeToRemove = node;
            break;
        }
    }

    if( nodeToRemove != NULL)
    {
        if(nodeToRemove->childNodes.size() > 0)
        {
            std::cout << "The directory is not empty: Can't be removed" << std::endl;
        }
        else
        {
            tree->removeNode(nodeToRemove);
        }
    }
    else
    {
        std::cout << "This directory doesn't exist" << std::endl;
    }
}

void Terminal::rm(command_t aCommand)
{
    std::string dirToRemove(aCommand.args->at(0));
    node_t* nodeToRemove = NULL;
    int index;

    for(index = 0; index < nodeToRemove->fatherNode->childNodes.size(); index++)
    {
        node_t* node = nodeToRemove->fatherNode->childNodes.at(index);
        if(dirToRemove == node->nameNode)
        {
            nodeToRemove = node;
            break;
        }
    }

    if( nodeToRemove != NULL)
    {
        tree->removeNode(nodeToRemove);
    }
    else
    {
        std::cout << "This file doesn't exist" << std::endl;
    }
}

void Terminal::upload(command_t aCommand)
{
    //TODO: No se si debe subir archivos y directorios del sistema local o puede crear archivos (touch) en el remoto con esto
    // y en el caso de subir un directorio, creará el directorio y luego nodos
}

void Terminal::mv(command_t aCommand)
{
    std::string nodeNameOrig(aCommand.args->at(0));
    std::string nodeNameDest(aCommand.args->at(1));

   node_t* targetNode = NULL;

    for(int index = 0; index < tree->getActualDirectoryNode()->childNodes.size(); index++)
    {
        node_t* node = tree->getActualDirectoryNode()->childNodes.at(index);
        if(nodeNameOrig == node->nameNode)
        {
            targetNode = node;
            break;
        }
    }

    if(targetNode != NULL)
    {
        tree->updateNode(targetNode->id, nodeNameDest, targetNode->size);
    }
    else
    {
        std::cout << "This file or directory doesn't exist" << std::endl;
    }

}

void Terminal::cp(command_t aCommand)
{
    std::string nodeNameOrigin(aCommand.args->at(0));

    char spacer[2]="/";
    char* token=NULL;

    //get node names of all path
    token=strtok(aCommand.args->at(1), spacer);

    while(token!=NULL){
        token=strtok(NULL, spacer);
        aCommand.args->push_back(token);
    }
}

void Terminal::lls()
{
    system("ls");
}

void Terminal::lcd(command_t aCommand)
{
    std::string commandLine = "cd ";
    std::string args(aCommand.args->at(0));
    commandLine += args;
    system(commandLine.c_str());
}

void Terminal::lpwd()
{
    system("pwd");
}

void Terminal::run(){
    bool exit=false;
    command_t command;
    command.args = new std::vector<char*>();
    while(!exit){
        std::cout << "$: ";
        readCommand(&command);
        runCommand(command);
    }
}
void Terminal::readCommand(command_t* aCommand){
    char* line=new char [1024];
    char spacer[2]=" ";
    char* token=NULL;

    fgets(line, 1023, stdin);
    token=strtok(line, spacer);
    aCommand->type=getTypeOfCommand(token);

    while(token!=NULL){
        token=strtok(NULL, spacer);
        aCommand->args->push_back(token);
    }
}

command_e Terminal::getTypeOfCommand(char* aCommandArray)
{
    if(aCommandArray != NULL)
    {
        if(strncmp("cd", aCommandArray, 2))
            return command_e::CD;
        else if(strncmp("ls", aCommandArray, 2))
            return command_e::LS;
        else if(strncmp("pwd", aCommandArray, 3))
            return command_e::PWD;
        else if(strncmp("mv", aCommandArray, 2))
            return command_e::MV;
        else if(strncmp("cp", aCommandArray, 2))
            return command_e::CP;
        else if(strncmp("mkdir", aCommandArray, 5))
            return command_e::MKDIR;
        else if(strncmp("rmdir", aCommandArray, 5))
            return command_e::RMDIR;
        else if(strncmp("rm", aCommandArray, 2))
            return command_e::RM;
        else if(strncmp("lls", aCommandArray, 3))
            return command_e::LLS;
        else if(strncmp("lcd", aCommandArray, 3))
            return command_e::LCD;
        else if(strncmp("lpwd", aCommandArray, 4))
            return command_e::LPWD;
        else if(strncmp("upload", aCommandArray, 6))
            return command_e::UPLOAD;
        else if(strncmp("exit", aCommandArray, 4))
            return command_e::END;

    }
    return command_e::NO_COMMAND;
}
void Terminal::runCommand(command_t aCommand)
{
    switch(aCommand.type)
    {
        case command_e::CD:
            break;
        case command_e::LS:
            break;
        case command_e::PWD:
            break;
        case command_e::MV:
            break;
        case command_e::CP:
            break;
        case command_e::MKDIR:
            break;
        case command_e::RMDIR:
            break;
        case command_e::RM:
            break;
        case command_e::LLS:
            break;
        case command_e::LCD:
            break;
        case command_e::LPWD:
            break;
        case command_e::UPLOAD:
            break;
        case command_e::END:
            break;
        case command_e::NO_COMMAND:
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
