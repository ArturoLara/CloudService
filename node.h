/*
 * Authors: Mario Cavero y Arturo Lara
*/

#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>

struct node_t
{
    unsigned int id;
    node_t* fatherNode;
    std::string nameNode;
    std::vector<node_t*> childNodes;
    unsigned int deepLevel;
    bool directoryFlag;
    off_t size;
    time_t lastChange;

    node_t(unsigned int id, node_t* fatherNode, std::string nameNode, unsigned int actualDeepLevel, bool directory, off_t size)
    {
        this->id = id;
        this->fatherNode = fatherNode;
        this->nameNode = nameNode;
        this->deepLevel = actualDeepLevel;
        this->directoryFlag = directory;
        this->size = size;
        time(&(this->lastChange));
    }

};

#endif // NODE_H
