#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>
#include <cstring>

Terminal::Terminal()
{
    tree = new DataTree();
}

void Terminal::cd(command_t aCommand){

    if(strncmp("..", aCommand.args[0], 2))
    {
        node_t* node = this->tree->getActualDirectoryNode()->fatherNode;
        this->tree->setActualDirectoryNode(node);
        completeFlag = true;
    }
    else if(strncmp("/", aCommand.args[0], 1))
    {
        this->tree->setActualDirectoryNode(tree->findNode(0));
        completeFlag = true;
    }
    else
    {
        bool completeFlag = false;
        for(node_t* node : this->tree->getActualDirectoryNode()->childNodes)
        {
            if(strncmp(command.args[0] , node->nameNode.c_str(), node->nameNode.size()))
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
    std::endl;
}

std::string Terminal::pwdRecursive(node_t aNode){

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


void Terminal::mkdir(command_t aCommand){

    std::string nameOfDir(aCommand.args[0]);
    tree->addNode(tree->getActualDirectoryNode(), nameOfDir, true, SIZE_OF_DIRECTORY);
}

void Terminal::rmdir(command_t aCommand){
    std::string dirToRemove(aCommand.args[0]);
    node_t nodeToRemove = NULL;
    int index;

    for(index = 0; index < nodeToRemove.fatherNode->childNodes.size(); index++)
    {
        node_t* node = nodeToRemove.fatherNode->childNodes.at(index);
        if(dirToRemove == node->nameNode)
        {
            nodeToRemove = node;
            break;
        }
    }

    if( nodeToRemove != NULL)
    {
        if(nodeToRemove.childNodes.size() > 0)
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
    std::string dirToRemove(aCommand.args[0]);
    node_t nodeToRemove = NULL;
    int index;

    for(index = 0; index < nodeToRemove.fatherNode->childNodes.size(); index++)
    {
        node_t* node = nodeToRemove.fatherNode->childNodes.at(index);
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
    std::string nodeNameOrig(aCommand.args[0]);
    std::string nodeNameDest(aCommand.args[1]);

   node_t targetNode = NULL;

    for(index = 0; index < tree->getActualDirectoryNode()->childNodes.size(); index++)
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
        tree->updateNode(targetNode.id, nodeNameDest, targetNode.size);
    }
    else
    {
        std::cout << "This file or directory doesn't exist" << std::endl;
    }

}

void Terminal::cp(command_t aCommand)
{
    std::string nodeNameOrigin(aCommand.args[0]);
    std::string pathNodoDestino(aCommand.args[1]);

    char spacer[2]="/";
    char* token=NULL;

    fgets(line, 1023, stdin);
    token=strtok(line, spacer);

    while(token!=NULL){
        token=strtok(NULL, spacer);
        aCommand->args->push_back(token);
    }
}

void Terminal::lls()
{
    system("ls");
}

void Terminal::lcd(command_t aCommand)
{
    std::string commandLine = "cd " + aCommand.args[1];
    system(commandLine.c_str());
}

void Terminal::lpwd()
{
    system("pwd");
}

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
void Terminal::lee_comando(command_t* aCommand){
    char* line=new char [1024];
    char spacer[2]=" ";
    char* token=NULL;

    fgets(line, 1023, stdin);
    token=strtok(line, spacer);
    aCommand->type=get_tipo_comando(token);

    while(token!=NULL){
        token=strtok(NULL, spacer);
        aCommand->args->push_back(token);
    }
}

command_e Terminal::get_tipo_comando(char* aCommandArray)
{
    if(commandArray != NULL)
    {
        if(strncmp("cd", aCommandArray, 2))
            return command_e::cd;
        else if(strncmp("ls", aCommandArray, 2))
            return command_e::ls;
        else if(strncmp("pwd", aCommandArray, 3))
            return command_e::pwd;
        else if(strncmp("mv", aCommandArray, 2))
            return command_e::mv;
        else if(strncmp("cp", aCommandArray, 2))
            return command_e::cp;
        else if(strncmp("mkdir", aCommandArray, 5))
            return command_e::mkdir;
        else if(strncmp("rmdir", aCommandArray, 5))
            return command_e::rmdir;
        else if(strncmp("rm", aCommandArray, 2))
            return command_e::rm;
        else if(strncmp("lls", aCommandArray, 3))
            return command_e::lls;
        else if(strncmp("lcd", aCommandArray, 3))
            return command_e::lcd;
        else if(strncmp("lpwd", aCommandArray, 4))
            return command_e::lpwd;
        else if(strncmp("upload", aCommandArray, 6))
            return command_e::upload;
        else if(strncmp("exit", aCommandArray, 4))
            return command_e::exit;

    }
    return command_e::noCommand;
}
void Terminal::ejecuta_comando(command_t* aCommand)
{
    switch(aCommand->type)
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
