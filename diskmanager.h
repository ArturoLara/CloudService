#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <vector>
#include <queue>
#include <string>
#include <sys/stat.h>

const off_t sizeOfBlock = 1024;
const int numDisks = 4;

class DiskManager
{
public:
    DiskManager(std::string pwd, int numBlocks);
    std::vector<std::pair<int,int>> writeFile(void* file, off_t sizeFile);
    void readFile(void* file, off_t sizeFile, std::vector<std::pair<int,int>> vectorOfBlocks);
    void removeFile(std::vector<std::pair<int,int>> vectorOfBlocks);
    void setNumBlocks(int newNumBlocks);
private:
    std::string pwdLocal;
    int numBlocks;
    std::vector<std::queue<int>> vectorOfFreeBlocksRAID;

    std::pair<int,int> writeBlock(void* block);
    void readBlock(void* block, std::pair<int,int> blockId);
    void format(int newNumberOfBlocks, int disk);
    void saveTables();
};

#endif // DISKMANAGER_H
