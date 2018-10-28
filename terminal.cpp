/*
 * Authors: Mario Cavero y Arturo Lara
*/

#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sstream>

Terminal::Terminal()
{
    tree = new DataTree();
}

void Terminal::cd(command_t aCommand){
    if(aCommand.args->size() == 2)
    {
        bool completeFlag = false;
        if(!strncmp("..", aCommand.args->at(0), 2))
        {
            node_t* node = this->tree->getActualDirectoryNode()->fatherNode;
            if(node != NULL)
            {
                this->tree->setActualDirectoryNode(node);
                completeFlag = true;
            }
        }
        else if(!strncmp("/", aCommand.args->at(0), 1))
        {
            tree->setActualDirectoryNode(tree->findNode(0));
            completeFlag = true;
        }
        else
        {
            for(node_t* node : this->tree->getActualDirectoryNode()->childNodes)
            {
                if(strlen(aCommand.args->at(0)) == node->nameNode.size())
                {
                    if(!strncmp(aCommand.args->at(0) , node->nameNode.c_str(), node->nameNode.size()))
                    {
                        if(node->directoryFlag)
                        {
                            this->tree->setActualDirectoryNode(node);
                            completeFlag = true;
                            break;
                        }
                    }
                }
            }
            if(!completeFlag)
            {
                std::cout << "It's not a directory" << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
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
        tm* timeinfo = localtime (&node->lastChange);
        std::cout << node->size << "Bytes " << asctime(timeinfo) << std::endl;
    }
}

node_t* Terminal::findDirectoryAtDirectory(node_t* directoryNode, std::string DirectoryName)
{
    node_t* nodeToReturn = NULL;
    for(int index = 0; index < directoryNode->childNodes.size(); index++)
    {
        node_t* node = directoryNode->childNodes.at(index);
        if(DirectoryName == node->nameNode)
        {
            if(node->directoryFlag)
            {
                nodeToReturn = node;
                break;
            }
        }
    }
    return nodeToReturn;
}

node_t* Terminal::findFileAtDirectory(node_t* directoryNode, std::string fileName)
{
    node_t* nodeToReturn = NULL;
    for(int index = 0; index < directoryNode->childNodes.size(); index++)
    {
        node_t* node = directoryNode->childNodes.at(index);
        if(fileName == node->nameNode)
        {
            if(!node->directoryFlag)
            {
                nodeToReturn = node;
                break;
            }
        }
    }
    return nodeToReturn;
}

node_t* Terminal::findNodeAtDirectory(node_t* directoryNode, std::string nodeName)
{
    node_t* nodeToReturn = NULL;
    for(int index = 0; index < directoryNode->childNodes.size(); index++)
    {
        node_t* node = directoryNode->childNodes.at(index);
        if(nodeName == node->nameNode)
        {
            nodeToReturn = node;
            break;
        }
    }
    return nodeToReturn;
}

void Terminal::copyDirectoryRecursive(node_t* OriginDirectory, node_t* destDirectory, std::string newNameDirectory)
{
    destDirectory = tree->addNode(destDirectory, newNameDirectory, OriginDirectory->directoryFlag, OriginDirectory->size);
    for(node_t* node : OriginDirectory->childNodes)
    {
        copyDirectoryRecursive(node, destDirectory, node->nameNode);
    }
}

void Terminal::copyLocalDirectoryRecursive(node_t* destDirectory)
{
    std::string token;
    std::string localItemList = getOutFromCommand("ls | sort");
    std::stringstream streamString(localItemList);

    while(std::getline(streamString, token, '\n'))
    {
        struct stat buffer;
        stat(token.c_str(), &buffer);
        if(S_ISREG(buffer.st_mode))
        {
            tree->addNode(destDirectory, token, false, buffer.st_size);
        }
        else
        {
            node_t* nextDir = tree->addNode(destDirectory, token, true, SIZE_OF_DIRECTORY);
            chdir(token.c_str());
            copyLocalDirectoryRecursive(nextDir);
            chdir("..");
        }
    }
}

std::string Terminal::getOutFromCommand(std::string cmd)
{
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");
    stream = popen(cmd.c_str(), "r");

    if(stream)
    {
        while(!feof(stream))
        {
            if(fgets(buffer, max_buffer, stream) != NULL)
            {
                data.append(buffer);
            }
        }
        pclose(stream);
    }
    return data;
}
bool Terminal::findSubdirectoryDependency(node_t* OriginDirectory, node_t* destDirectory)
{
    bool dependency = false;

    if(OriginDirectory->id == destDirectory->id)
    {
        dependency = true;
    }

    for(node_t* node : destDirectory->childNodes)
    {
        dependency = findSubdirectoryDependency(OriginDirectory, node);
    }

    return dependency;
}

node_t* Terminal::findByPath(char *command){
    bool dirFlag = false;
    if(!strncmp(&(command[strlen(command)-1]), "/", 1))
    {
        dirFlag = true;
    }
    if(!strncmp(&(command[0]), "/", 1))
    {
        return findByPathRecursive(command, tree->findNode(0), 1, dirFlag);
    }
    else
    {
        return findByPathRecursive(command, tree->getActualDirectoryNode(), 0, dirFlag);
    }

}

node_t* Terminal::findByPathRecursive(char *path, node_t* actualDirectory, int index, bool dirFlag){


    char* actualPath=strtok(path+index, "/\n");
    char* nextPath = strtok(NULL , "/");

    std::string nextDir(actualPath);

    node_t* targetDirectory = findDirectoryAtDirectory(actualDirectory, nextDir);

    index += strlen(actualPath)+1;

    if(targetDirectory != NULL)
    {
        if(nextPath != NULL)
        {
            return findByPathRecursive(path, targetDirectory, index, dirFlag);
        }
        else
        {
            if(dirFlag)
            {
                return targetDirectory;
            }
            else
            {
                return actualDirectory;
            }
        }
    }
    else
    {
        if(nextPath != NULL)
        {
            return NULL;
        }
        else
        {
            if(dirFlag)
            {
                return NULL;
            }
            else
            {
                return actualDirectory;
            }
        }
    }
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
    if(aCommand.args->size() == 2)
    {
        std::string nameOfDir(aCommand.args->at(0));
        if(std::string::npos == nameOfDir.find("/"))
        {
            if(findDirectoryAtDirectory(tree->getActualDirectoryNode(), nameOfDir) == NULL)
            {
                tree->addNode(tree->getActualDirectoryNode(), nameOfDir, true, SIZE_OF_DIRECTORY);
            }
            else
            {
                std::cout << "Error: You can not use the same name as other directory" << std::endl;
            }
        }
        else
        {
            std::cout << "Error: You can not use \"/\"" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }

}

void Terminal::rmdir(command_t aCommand){
    if(aCommand.args->size() == 2)
    {
        std::string dirToRemove(aCommand.args->at(0));

        node_t* nodeToRemove = NULL;
        int index;
        for(index = 0; index < tree->getActualDirectoryNode()->childNodes.size(); index++)
        {
            node_t* node = tree->getActualDirectoryNode()->childNodes.at(index);
            if(dirToRemove == node->nameNode)
            {
                if(node->directoryFlag)
                {
                    nodeToRemove = node;
                }
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
            std::cout << "This directory doesn't exist or is a file" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }
}

void Terminal::rm(command_t aCommand)
{
    if(aCommand.args->size() == 2)
    {
        std::string fileToRemove(aCommand.args->at(0));
        node_t* nodeToRemove = NULL;

        nodeToRemove = findFileAtDirectory(tree->getActualDirectoryNode(), fileToRemove);

        if( nodeToRemove != NULL)
        {
            tree->removeNode(nodeToRemove);
        }
        else
        {
            std::cout << "This file doesn't exist" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }
}

void Terminal::upload(command_t aCommand)
{
    if(aCommand.args->size() == 2)
    {
        std::string localNameToUpload(aCommand.args->at(0));

        struct stat buffer;
        stat(aCommand.args->at(0), &buffer);
        if(S_ISREG(buffer.st_mode))
        {
            tree->addNode(tree->getActualDirectoryNode(), localNameToUpload, false, buffer.st_size);
        }
        else
        {
            node_t* nextDir = tree->addNode(tree->getActualDirectoryNode(), localNameToUpload, true, SIZE_OF_DIRECTORY);
            chdir(aCommand.args->at(0));
            copyLocalDirectoryRecursive(nextDir);
            chdir("..");
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }
}

void Terminal::mv(command_t aCommand)
{
    if(aCommand.args->size() == 3)
    {
        std::string nodeNameOrig(aCommand.args->at(0));
        std::string nodeNameDest(aCommand.args->at(1));
        node_t* targetNode = NULL;

        targetNode = findNodeAtDirectory(tree->getActualDirectoryNode(), nodeNameOrig);

        if(targetNode != NULL)
        {
            if(targetNode->directoryFlag && findDirectoryAtDirectory(tree->getActualDirectoryNode(), nodeNameDest) == NULL)
            {
                tree->updateNode(targetNode->id, nodeNameDest, targetNode->size);
            }
            else if(!targetNode->directoryFlag && findFileAtDirectory(tree->getActualDirectoryNode(), nodeNameDest) == NULL)
            {
                tree->updateNode(targetNode->id, nodeNameDest, targetNode->size);
            }
            else
            {
                std::cout << "Error: You can not use same name for 2 files or directories" << std::endl;
            }
        }
        else
        {
            std::cout << "This file or directory doesn't exist" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }
}

void Terminal::cp(command_t aCommand)
{

    if(aCommand.args->size() == 3)
    {
        std::string nodeNameOrigin(aCommand.args->at(0));
        std::string destNodeName;
        char* destName;
        node_t* originNode = NULL;
        node_t* destNode = NULL;

        char* destPathCopy;
        strcpy(destPathCopy, aCommand.args->at(1));

        if(!strncmp(&(destPathCopy[strlen(destPathCopy)-1]), "/", 1))
        {
            destNodeName = nodeNameOrigin;
        }
        else
        {
            char* token;

            token = strtok(destPathCopy, "/\n");
            while(token != NULL)
            {
                destName = token;
                token = strtok(NULL, "/\n");
            }
            destNodeName = std::string(destName);
        }

        originNode = findDirectoryAtDirectory(tree->getActualDirectoryNode(), nodeNameOrigin);
        destNode = findByPath(aCommand.args->at(1));

        if(originNode != NULL && destNode != NULL)
        {
            if(originNode->directoryFlag)
            {
                if(findDirectoryAtDirectory(destNode, destNodeName) == NULL)
                {
                    if(!findSubdirectoryDependency(originNode, destNode))
                    {
                        copyDirectoryRecursive(originNode, destNode, destNodeName);
                    }
                    else
                    {
                        std::cout << "Error: Can not copy a directory in one of its subfolders" << std::endl;
                    }

                }
                else
                {
                    std::cout << "Error: You can not use same name for 2 directories" << std::endl;
                }
            }
            else
            {
                if(findFileAtDirectory(destNode, destNodeName) == NULL)
                {
                    tree->addNode(destNode, destNodeName, false, originNode->size);
                }
                else
                {
                    std::cout << "Error: You can not use same name for 2 files" << std::endl;
                }

            }
        }
        else
        {
           std::cout << "This file or directory doesn't exist" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }

}

void Terminal::lls()
{
    system("ls");
}

void Terminal::lcd(command_t aCommand)
{
    if(aCommand.args->size() == 2)
    {
        std::string args(aCommand.args->at(0));
        chdir(args.c_str());
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }
}

void Terminal::lpwd()
{
    system("pwd");
}

void Terminal::run(){
    command_t command;
    command.args = new std::vector<char*>();
    tree->addNode(tree->addNode(tree->getActualDirectoryNode(), "a", true, 2048), "b", true, 2048);
    tree->addNode(tree->addNode(tree->getActualDirectoryNode(), "c", true, 2048), "d", true, 2048);

    while(!exit){
        command.clean();
        std::cout << "$: ";
        readCommand(&command);
        runCommand(command);
    }
}
void Terminal::readCommand(command_t* aCommand){
    char * line= new char [1024];
    char spacer[3]=" \n";
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
        if(!strncmp("cd", aCommandArray, 2))
            return command_e::CD;
        else if(!strncmp("ls", aCommandArray, 2))
            return command_e::LS;
        else if(!strncmp("pwd", aCommandArray, 3))
            return command_e::PWD;
        else if(!strncmp("mv", aCommandArray, 2))
            return command_e::MV;
        else if(!strncmp("cp", aCommandArray, 2))
            return command_e::CP;
        else if(!strncmp("mkdir", aCommandArray, 5))
            return command_e::MKDIR;
        else if(!strncmp("rmdir", aCommandArray, 5))
            return command_e::RMDIR;
        else if(!strncmp("rm", aCommandArray, 2))
            return command_e::RM;
        else if(!(strncmp("lls", aCommandArray, 3)))
            return command_e::LLS;
        else if(!strncmp("lcd", aCommandArray, 3))
            return command_e::LCD;
        else if(!strncmp("lpwd", aCommandArray, 4))
            return command_e::LPWD;
        else if(!strncmp("upload", aCommandArray, 6))
            return command_e::UPLOAD;
        else if(!strncmp("exit", aCommandArray, 4))
            return command_e::END;
    }
    return command_e::NO_COMMAND;
}
void Terminal::runCommand(command_t aCommand)
{
    switch(aCommand.type)
    {
        case command_e::CD:
            cd(aCommand);
            break;
        case command_e::LS:
            ls();
            break;
        case command_e::PWD:
            pwd();
            break;
        case command_e::MV:
            mv(aCommand);
            break;
        case command_e::CP:
            cp(aCommand);
            break;
        case command_e::MKDIR:
            mkdir(aCommand);
            break;
        case command_e::RMDIR:
            rmdir(aCommand);
            break;
        case command_e::RM:
            rm(aCommand);
            break;
        case command_e::LLS:
            lls();
            break;
        case command_e::LCD:
            lcd(aCommand);
            break;
        case command_e::LPWD:
            lpwd();
            break;
        case command_e::UPLOAD:
            upload(aCommand);
            break;
        case command_e::END:
            exit = true;
            break;
        case command_e::NO_COMMAND:
            std::cout << "Command not found" << std::endl;
            break;
        default:
            std::cout << "Command not found" << std::endl;
            break;
    }
}
