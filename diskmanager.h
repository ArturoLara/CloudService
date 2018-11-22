#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <vector>
#include <list>
#include <string>
#include <sys/stat.h>

const off_t sizeOfBlock = 1024;
const int numDisks = 4;

class DiskManager
{
public:
    DiskManager(std::string pwd, int numBlocks);
    std::vector<std::pair<int,int>> writeFile(char* file, off_t sizeFile);
    void readFile(char* file, off_t sizeFile, std::vector<std::pair<int,int>> vectorOfBlocks);
    void removeFile(std::vector<std::pair<int,int>> vectorOfBlocks);
    void setNumBlocks(int newNumBlocks);
private:
    std::string pwdLocal;
    int numBlocks;
    off_t spaceInRAID;
    std::vector<std::list<int>> vectorOfFreeBlocksRAID;

    std::pair<int,int> writeBlock(char* block);
    void readBlock(char* block, std::pair<int,int> blockId);
    void format(int newNumberOfBlocks, int disk);
    void saveTables();
};

#endif // DISKMANAGER_H
