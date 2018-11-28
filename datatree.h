/*
 * Authors: Mario Cavero y Arturo Lara
*/

#ifndef DATATREE_H
#define DATATREE_H

#include <string>
#include <vector>
#include <sys/stat.h>

#include "node.h"

const unsigned int INITIAL_NODE_ID = 0;
const unsigned int INITIAL_DEEP_LEVEL = 0;
const off_t SIZE_OF_DIRECTORY = 4096;
const int SIZE_OF_BLOCK = 46;

class DataTree
{
public:
    DataTree(std::string pwd);
    node_t* addNode(node_t* aFatherNode, std::string aNameNode, bool aDirectory, off_t aSize, std::vector<int> vectorOfBlocksId = std::vector<int>());
    node_t* findNode(unsigned int aIdNode);
    void updateNode(unsigned int aIdNode, std::string aNameNode, off_t size);
    void removeNode(node_t* aIdNode);
    inline node_t* getActualDirectoryNode(){ return this->actualDirectoryNode; }
    inline void setActualDirectoryNode(node_t* node){ this->actualDirectoryNode = node; }
    void saveTree();
    void loadTree();
private:
    std::string pwdLocal;
    node_t* rootNode;
    node_t* actualDirectoryNode;
    unsigned int nodeCount;
    unsigned int lastIdNode;

    node_t* findNodeRecursive(unsigned int aIdNode, node_t* rootNode);
    void saveTreeRecursive(node_t* node, FILE* file);
    node_t* loadTreeRecursive(node_t* node, FILE* file, unsigned int totalNodes);
};

#endif // DATATREE_H
