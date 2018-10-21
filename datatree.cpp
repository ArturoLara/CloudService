#include "datatree.h"

#include <algorithm>
#include <iostream>

DataTree::DataTree()
{
    node_t* aRootNode = new node_t(INITIAL_NODE_ID, NULL, "root", INITIAL_DEEP_LEVEL, true, SIZE_OF_DIRECTORY);
    rootNode = aRootNode;
    actualDirectoryNode = aRootNode;
    nodeCount = 1;
    lastIdNode = INITIAL_NODE_ID;
}

void DataTree::addNode(node_t* aFatherNode, std::string aNameNode, bool aDirectory, off_t aSize)
{
    unsigned int newDeepLevel = aFatherNode->deepLevel + 1;
        std::cout << "2" << std::endl;
    node_t* newNode = new node_t(nodeCount, aFatherNode, aNameNode, newDeepLevel, aDirectory, aSize);
        std::cout << "3" << std::endl;
    aFatherNode->childNodes.push_back(newNode);
    lastIdNode = nodeCount;
    nodeCount += 1;
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
