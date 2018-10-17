#ifndef DATATREE_H
#define DATATREE_H

#include <string>
#include <vector>
#include "sys/stat.h>

#include "node.h"

const unsigned int INITIAL_NODE_ID = 0;
const unsigned int INITIAL_DEEP_LEVEL = 0;
const off_t SIZE_OF_DIRECTORY = 4096;

class DataTree
{
public:
    DataTree();

    void addNode(node_t* aFatherNode, std::string aNameNode, bool aDirectory, off_t aSize);
    node_t* findNode(unsigned int aIdNode);
    void updateNode(unsigned int aIdNode, std::string aNameNode, off_t size);
    void removeNode(unsigned int aIdNode);
    inline node_t* getActualDirectoryNode(){ return this->actualDirectoryNode; }
    inline void setActualDirectoryNode(node_t* node){ this->actualDirectoryNode = node; }

private:

    node_t* rootNode;
    node_t* actualDirectoryNode;
    unsigned int nodeCount;
    unsigned int lastIdNode;

    node_t* findNodeRecursive(unsigned int aIdNode, node_t rootNode);
};

#endif // DATATREE_H
