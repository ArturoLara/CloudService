#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <vector>
#include <list>
#include <string>
#include <sys/stat.h>
#include "/usr/include/mpi/mpi.h"

const off_t sizeOfBlock = 1024;
const int numDisks = 4;

enum function_e
{
    WRITE = 1,
    READ = 2
};

class DiskManager
{
public:
    DiskManager(std::string pwd, int numBlocks);
    std::vector<int> writeFile(char* file, off_t sizeFile);
    void readFile(char* file, off_t sizeFile, std::vector<int> vectorOfBlocks);
    void removeFile(std::vector<int> vectorOfBlocks);
    void setNumBlocks(int newNumBlocks);
    void format(int numBlocks);

private:
    std::string pwdLocal;
    int numBlocks;
    off_t spaceInRAID;
    std::list<int> vectorOfFreeBlocksRAID;
    std::vector<MPI_Comm*> diskVector;

    void saveTables();
};

#endif // DISKMANAGER_H
