#include "diskmanager.h"

#include <cstring>
#include <string>
#include <iostream>

DiskManager::DiskManager(std::string pwd, int numBlocks)
{
    this->numBlocks = numBlocks;
    pwdLocal = pwd;
    spaceInRAID = (numDisks*numBlocks*sizeOfBlock);
    for(int i = 0; i < numDisks; i++)
    {
        std::cout << i << std::endl;
        std::string diskName = pwdLocal;
        diskName += "/disco";
        diskName += std::to_string(i);
        diskName += ".dat";

        std::string freeSectors = pwdLocal;
        freeSectors += "/sectoresLibres";
        freeSectors += std::to_string(i);
        freeSectors += ".dat";

        FILE* sectorsFile;
        FILE* diskFile = fopen(diskName.c_str(), "rb");

        if(diskFile == NULL)
        {
            std::cout << "No se ha encontrado el disco " << i << ", se creará un nuevo disco" << std::endl;
            FILE* tmp = fopen(diskName.c_str(), "wb");
            fclose(tmp);
            format(numBlocks, i);
        }
        else
        {
            fclose(diskFile);

            sectorsFile = fopen(freeSectors.c_str(), "rb");
            if(sectorsFile == NULL)
            {
                std::cout << "ERROR: Disco " << i << " corrupto: no se ha encontrado su archivo de indices";
                format(numBlocks, i);

            }
            else
            {
                fclose(sectorsFile);
            }
        }
        vectorOfFreeBlocksRAID.push_back(std::list<int>());

        sectorsFile = fopen(freeSectors.c_str(), "rb");
        int numFreeBlocks, tmpBlockID;
        fread(&numFreeBlocks, sizeof(int), 1, sectorsFile);
        for(int j = 0; j < numFreeBlocks; j++)
        {
            fread(&tmpBlockID ,sizeof(int), 1, sectorsFile);
            vectorOfFreeBlocksRAID[i].push_back(tmpBlockID);
        }

        fclose(sectorsFile);
    }
}

std::vector<std::pair<int,int>> DiskManager::writeFile(char* file, off_t sizeFile)
{
    std::vector<std::pair<int,int>> usedBlocks;

    char* normalizedData = NULL;
    char* dataBock = (char*)malloc(sizeOfBlock*sizeof(char));
    int numOfBlocks = sizeFile/sizeOfBlock;
    int sizeInDisk = sizeFile+(sizeOfBlock-(sizeFile%sizeOfBlock));

    if(sizeInDisk < spaceInRAID)
    {
        if(sizeFile%sizeOfBlock != 0)
        {
            normalizedData = (char*)malloc(sizeInDisk*sizeof(char));
            memcpy(normalizedData, file, sizeFile);
            for(int i = sizeFile; i <= sizeInDisk; i++)
            {
                *(normalizedData+i) = '\0';
            }
            numOfBlocks += 1;
        }

        for(int i = 0; i < numOfBlocks; i++)
        {
            memcpy(dataBock, normalizedData+(i*sizeOfBlock), sizeof(char)*sizeOfBlock);
            std::pair<int,int> tmpBlockID = writeBlock(dataBock);
            usedBlocks.push_back(tmpBlockID);
            if(tmpBlockID.second == -1)
            {
                break;
            }
        }
        spaceInRAID -= sizeInDisk;
        saveTables();

        free(dataBock);
        if(normalizedData != NULL)
            free(normalizedData);
    }
    else
    {
        std::cout << "No hay espacio suficiente, libere " << ((sizeInDisk-spaceInRAID)/(1024*1024)) << " Mb" << std::endl;
    }
    return usedBlocks;

}

void DiskManager::readFile(char* file, off_t sizeFile, std::vector<std::pair<int,int>> vectorOfBlocks)
{
    char* normalizedData = (char*)malloc(vectorOfBlocks.size()*sizeOfBlock);

    char* dataBock = (char*)malloc(sizeOfBlock*sizeof(char));

    for(int i = 0; i < vectorOfBlocks.size(); i++)
    {
        readBlock(dataBock, vectorOfBlocks[i]);
        memcpy(normalizedData+(i*sizeOfBlock), dataBock, sizeof(char)*sizeOfBlock);
    }

    memcpy(file, normalizedData, sizeFile);

    free(dataBock);
    free(normalizedData);
}

std::pair<int,int> DiskManager::writeBlock(char* block)
{
    int mayorSize = -1, diskToUse = -1;
    for(int i = 0; i < vectorOfFreeBlocksRAID.size(); i++)
    {
        int tempSize = vectorOfFreeBlocksRAID[i].size();
        if(mayorSize < tempSize)
        {
            mayorSize = vectorOfFreeBlocksRAID[i].size();
            diskToUse = i;
        }
    }
    if(diskToUse != -1)
    {
        int blockID = vectorOfFreeBlocksRAID[diskToUse].front();
        vectorOfFreeBlocksRAID[diskToUse].pop_front();
        std::string diskName = pwdLocal;
        diskName += "/disco";
        diskName += std::to_string(diskToUse);
        diskName += ".dat";

        FILE* diskFile = fopen(diskName.c_str(), "ab");
        if(diskFile != NULL)
        {
            fseek(diskFile, (blockID-1)*sizeOfBlock, SEEK_SET);
            fwrite(block ,sizeof(char), sizeOfBlock, diskFile);
            fclose(diskFile);
            std::pair<int,int> pairBlock = std::pair<int,int>(diskToUse, blockID);
            return pairBlock;
        }
    }
    else
    {
        std::cout << "No hay espacio en el sistema" << std::endl;
    }

    return std::pair<int, int>();
}

void DiskManager::readBlock(char* block, std::pair<int,int> blockId)
{
    int diskToUse = blockId.first;

    std::string diskName = pwdLocal;
    diskName += "/disco";
    diskName += std::to_string(diskToUse);
    diskName += ".dat";

    FILE* diskFile = fopen(diskName.c_str(), "rb");
    if(diskFile != NULL)
    {
        fseek(diskFile, (blockId.second-1)*sizeOfBlock, SEEK_SET);
        fread(block ,sizeof(char), sizeOfBlock, diskFile);
        fclose(diskFile);
    }
    else
    {
        std::cout << "ERROR: Disco " << diskToUse << " perdido" << std::endl;
    }
}

void DiskManager::format(int newNumberOfBlocks, int disk)
{
    if(disk == -1)
    {
        for(int i = 0; i < numDisks; i++)
        {
            std::string freeSectors = pwdLocal;
            freeSectors += "/sectoresLibres";
            freeSectors += std::to_string(i);
            freeSectors += ".dat";

            FILE* sectorsFile = fopen(freeSectors.c_str(), "wb");

            fwrite(&newNumberOfBlocks, sizeof(int), 1, sectorsFile);
            for(int j = 1; j <= newNumberOfBlocks; j++)
            {
                fwrite(&j, sizeof(int), 1, sectorsFile);
            }
            fclose(sectorsFile);
        }
    }
    else
    {
        if(newNumberOfBlocks != numBlocks)
            std::cout << "Se usará el mismo numero de bloques que los discos existentes, si quiere cambiarlo formatee todos los discos" << std::endl;

        std::string freeSectors = pwdLocal;
        freeSectors += "/sectoresLibres";
        freeSectors += std::to_string(disk);
        freeSectors += ".dat";

        FILE* sectorsFile = fopen(freeSectors.c_str(), "wb");

        fwrite(&numBlocks, sizeof(int), 1, sectorsFile);
        for(int j = 1; j <= numBlocks; j++)
        {
            fwrite(&j, sizeof(int), 1, sectorsFile);
        }

        fclose(sectorsFile);
    }

}

void DiskManager::saveTables()
{
    for(int i = 0; i < numDisks; i++)
    {
        std::string freeSectors = pwdLocal;
        freeSectors += "/sectoresLibres";
        freeSectors += std::to_string(i);
        freeSectors += ".dat";

        FILE* sectorsFile = fopen(freeSectors.c_str(), "wb");

        int numFreeBlocks = vectorOfFreeBlocksRAID[i].size();

        fwrite(&numFreeBlocks, sizeof(int), 1, sectorsFile);
        for(std::list<int>::iterator it = vectorOfFreeBlocksRAID[i].begin(); it != vectorOfFreeBlocksRAID[i].end(); ++it)
        {
            int tmpBlockID = *it;
            fwrite(&tmpBlockID ,sizeof(int), 1, sectorsFile);
        }
        fclose(sectorsFile);
    }
}

void DiskManager::setNumBlocks(int newNumBlocks)
{
    numBlocks = newNumBlocks;

    format(newNumBlocks, -1);
}

void DiskManager::removeFile(std::vector<std::pair<int,int>> vectorOfBlocks)
{
    for(std::pair<int, int> pairBlock : vectorOfBlocks)
    {
        vectorOfFreeBlocksRAID[pairBlock.first].push_front(pairBlock.second);
    }

    saveTables();
}