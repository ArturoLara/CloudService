#ifndef NODE_H
#define NODE_H


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

    void node_t(unsigned int id, node_t* fatherNode, std::string nameNode, unsigned int actualDeepLevel, bool directory, off_t size)
    {
        this->id = id;
        this->fatherNode = fatherNode;
        this->nameNode = nameNode;
        this->deepLevel = actualDeepLevel;
        this->directoryFlag = directory;
        if(directory)
        {
            this->size = static_cast<off_t>(4096);
        }
        else
        {
            this->size = size;
        }
        time(&(this->lastChange));
    }

};

#endif // NODE_H
