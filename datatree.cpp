/*
 * Authors: Mario Cavero y Arturo Lara
*/

#include "datatree.h"

#include <algorithm>
#include <iostream>
#include <cstring>
DataTree::DataTree(std::string pwd)
{
    node_t* aRootNode = new node_t(INITIAL_NODE_ID, NULL, "root", INITIAL_DEEP_LEVEL, true, SIZE_OF_DIRECTORY);
    rootNode = aRootNode;
    actualDirectoryNode = aRootNode;
    nodeCount = 1;
    lastIdNode = INITIAL_NODE_ID;
    pwdLocal = pwd;
}

void DataTree::saveTree()
{
    FILE* file;
    std::string pathToBackup = pwdLocal;
    pathToBackup += "/arbol.dat";
    file = fopen(pathToBackup.c_str(), "wb");
    fwrite(&nodeCount, sizeof(unsigned int), 1, file);
    fwrite(&lastIdNode, sizeof(unsigned int), 1, file);

    if(!rootNode->childNodes.empty())
    {
        for(node_t* childNode : rootNode->childNodes)
        {
            saveTreeRecursive(childNode, file);
        }
    }

    fclose(file);
}

void DataTree::saveTreeRecursive(node_t* node, FILE* file)
{
    char name[25];
    int size = node->vectorOfBlocksId.size();
    strcpy(name, node->nameNode.c_str());
    name[25] = '\0';
    fwrite(name,sizeof(char),sizeof(name), file);
    fwrite(&(node->deepLevel), sizeof(unsigned int), 1, file);
    fwrite(&(node->directoryFlag), sizeof(bool), 1, file);
    fwrite(&(node->lastChange), sizeof(time_t), 1, file);
    fwrite(&(node->size), sizeof(off_t), 1, file);
    fwrite(&size, sizeof(int), 1, file);

    for(int i = 0; i < node->vectorOfBlocksId.size(); i++)
    {
        fwrite(&(node->vectorOfBlocksId[i]), sizeof(int), 1, file);
    }

    if(!node->childNodes.empty())
    {
        for(node_t* childNode : node->childNodes)
        {
            saveTreeRecursive(childNode, file);
        }
    }
}

void DataTree::loadTree()
{
    FILE* file;
    unsigned int lastID, count;

    file = fopen("arbol.dat", "rb");
    if(file != NULL)
    {
        fread(&count, sizeof(unsigned int), 1, file);
        fread(&lastID, sizeof(unsigned int), 1, file);

        loadTreeRecursive(rootNode, file, count);

        lastIdNode = lastID;

        fclose(file);
    }
    else
    {
        std::cout << "Not tree backup found, nothing loaded" << std::endl;
    }

}

node_t* DataTree::loadTreeRecursive(node_t* node, FILE* file, unsigned int totalNodes)
{
    if(nodeCount < totalNodes)
    {
        char name[25];
        unsigned int depth;
        bool flagDir;
        time_t lastChange;
        off_t size;
        int numBlocks, blockId;
        std::vector<int> vectorBlockId;

        fread(&name, sizeof(char), sizeof(name), file);
        fread(&depth, sizeof(unsigned int), 1, file);
        fread(&flagDir, sizeof(bool), 1, file);
        fread(&lastChange, sizeof(time_t), 1, file);
        fread(&size, sizeof(off_t), 1, file);
        fread(&numBlocks, sizeof(int), 1, file);

        for(int i = 0; i < numBlocks; i++)
        {
            fread(&blockId, sizeof(int), 1, file);
            vectorBlockId.push_back(blockId);
        }

        node_t* newNode = new node_t(nodeCount, node, name, depth, flagDir, size);
        newNode->vectorOfBlocksId = vectorBlockId;
        newNode->lastChange = lastChange;
        nodeCount++;

        while(node->deepLevel < newNode->deepLevel)
        {
            newNode->fatherNode = node;
            node->childNodes.push_back(newNode);
            newNode = loadTreeRecursive(newNode, file, totalNodes);
            if(newNode == NULL)
            {
                break;
            }
        }
        return newNode;
    }
    else
    {
        return NULL;
    }
}

node_t* DataTree::addNode(node_t* aFatherNode, std::string aNameNode, bool aDirectory, off_t aSize, std::vector<int> vectorOfBlocksId)
{
    unsigned int newDeepLevel = aFatherNode->deepLevel + 1;
    node_t* newNode = new node_t(nodeCount, aFatherNode, aNameNode, newDeepLevel, aDirectory, aSize);

    if(!aDirectory)
    {
        newNode->vectorOfBlocksId = vectorOfBlocksId;
    }

    aFatherNode->childNodes.push_back(newNode);
    lastIdNode = nodeCount;
    nodeCount += 1;
    return newNode;
}

node_t* DataTree::findNodeRecursive(unsigned int aIdNode, node_t* rootNode)
{
    node_t* foundNode = NULL;

    if(rootNode->id == aIdNode)
    {
        foundNode = rootNode;
    }
    else
    {
        for(node_t* node : rootNode->childNodes)
        {
            foundNode = findNodeRecursive(aIdNode, node);
            if(foundNode != NULL)
            {
                break;
            }
        }
    }

    return foundNode;
}

node_t* DataTree::findNode(unsigned int aIdNode)
{
    return findNodeRecursive(aIdNode, rootNode);
}

void DataTree::updateNode(unsigned int aIdNode, std::string aNameNode, off_t aSize)
{
    node_t* nodeToModify = findNode(aIdNode);
    nodeToModify->nameNode = aNameNode;
    nodeToModify->size = aSize;
    time(&(nodeToModify->lastChange));
}

void DataTree::removeNode(node_t* nodeToRemove)
{
    auto it = std::find(actualDirectoryNode->childNodes.begin(), actualDirectoryNode->childNodes.end(), nodeToRemove);
    if (it != actualDirectoryNode->childNodes.end())
    {
      actualDirectoryNode->childNodes.erase(it);
    }
    delete(nodeToRemove);
}
