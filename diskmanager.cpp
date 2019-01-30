#include "diskmanager.h"

#include <cstring>
#include <string>
#include <iostream>
/*!
 * \brief DiskManager::DiskManager El constructor de la clase que va a iniciar los discos y la comunicación
 * \param pwd la direccion total
 * \param numBlocks el número de bloques
 * Lo primero que haremos será iniciar la comunicación entre maestro y esclavos.
 * Hecho esto, inicializaremos los espacios de bloque por disco
 */
DiskManager::DiskManager(std::string pwd, int numBlocks)
{
    this->numBlocks = numBlocks;
    pwdLocal = pwd;

    int rank=0;
    int size=0;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    for(int i = 0; i < numDisks; i++)
    {
        MPI_Comm* commDisk=new MPI_Comm[1];
	MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_spawn("/home/ubuntu/MarioCavero_ArturoLara/bin/disk", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, commDisk, MPI_ERRCODES_IGNORE);
        MPI_Send(&i, 1, MPI_INT, 0, 0, *commDisk);
        diskVector.push_back(commDisk);
    }

    std::string freeSectors = pwdLocal;
    freeSectors += "/sectoresLibres";
    freeSectors += ".dat";

    FILE* sectorsFile;

    sectorsFile = fopen(freeSectors.c_str(), "rb");
    if(sectorsFile == NULL)
    {
        std::cout << "[ERROR] No se ha encontrado archivo de indices" << std::endl;
        format(numBlocks*numDisks);
    }
    else
    {
        fclose(sectorsFile);
    }

    sectorsFile = fopen(freeSectors.c_str(), "rb");
    int numFreeBlocks, tmpBlockID;
    fread(&numFreeBlocks, sizeof(int), 1, sectorsFile);
    spaceInRAID = (numDisks*numFreeBlocks*sizeOfBlock);
    for(int j = 0; j < numFreeBlocks; j++)
    {
        fread(&tmpBlockID ,sizeof(int), 1, sectorsFile);
        vectorOfFreeBlocksRAID.push_back(tmpBlockID);
    }

    fclose(sectorsFile);
}
/*!
 * \brief DiskManager::format es el método para el formateo de bloques por disco
 * \param numBlocks es el número de bloques
 * Guardaremos los sectores libres con el numero de bloques.
 */
void DiskManager::format(int numBlocks)
{
    std::string freeSectors = pwdLocal;
    freeSectors += "/sectoresLibres.dat";

    FILE* sectorsFile = fopen(freeSectors.c_str(), "wb");

    fwrite(&numBlocks, sizeof(int), 1, sectorsFile);
    for(int j = 0; j < numBlocks; j++)
    {
        fwrite(&j, sizeof(int), 1, sectorsFile);
    }

    fclose(sectorsFile);
}
/*!
 * \brief DiskManager::writeFile Método para escribir un fichero en disco
 * \param file el fichero
 * \param sizeFile el tamaño del fichero
 * \return usedblocks los bloques usados
 * Para escribir un fichero en disco, es importante saber que vamos a ir escribiendolo de manera equilibrada
 * en los distintos discos y en bloques.
 * Reservaremos la memoria suficiente que necesitamos por disco para el archivo.
 * enviaremos la distinta información por numero de bloques a los esclavos para saber qué tienen que guardar en qué bloque
 */
std::vector<int> DiskManager::writeFile(char* file, off_t sizeFile)
{
    std::vector<int> usedBlocks;

    char* normalizedData = NULL;
    char* dataBock = (char*)malloc(sizeOfBlock*sizeof(char));
    int numOfBlocks = sizeFile/sizeOfBlock;
    int sizeInDisk = sizeFile+(sizeOfBlock-(sizeFile%sizeOfBlock));
    std::cout << spaceInRAID << " " << sizeInDisk << std::endl;
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

            int tmpBlockID = vectorOfFreeBlocksRAID.front();
            int remoteBlockId = tmpBlockID/numDisks;
            int remoteFunc = WRITE;
            vectorOfFreeBlocksRAID.pop_front();
            memcpy(dataBock, normalizedData+(i*sizeOfBlock), sizeof(char)*sizeOfBlock);
            MPI_Send(&remoteFunc, 1, MPI_INT, 0, 0, *(diskVector[tmpBlockID%numDisks]));
            MPI_Send(&remoteBlockId, 1, MPI_INT, 0, 0, *(diskVector[tmpBlockID%numDisks]));
            MPI_Send(dataBock, sizeOfBlock, MPI_CHAR, 0, 0, *(diskVector[tmpBlockID%numDisks]));
            usedBlocks.push_back(tmpBlockID);
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
/*!
 * \brief DiskManager::readFile Método para leer un fichero
 * \param file el fichero
 * \param sizeFile el tamaño del fichero
 * \param vectorOfBlocks el numero de bloques
 * Les enviamos a los esclavos que van a tener que leer de sus bloques, y leemos el fichero.
 * De estos vamos a recibir el fichero leido.
 */
void DiskManager::readFile(char* file, off_t sizeFile, std::vector<int> vectorOfBlocks)
{
    char* normalizedData = (char*)malloc(vectorOfBlocks.size()*sizeOfBlock);

    char* dataBock = (char*)malloc(sizeOfBlock*sizeof(char));

    for(int i = 0; i < vectorOfBlocks.size(); i++)
    {
        MPI_Status status;
        int remoteBlockId = vectorOfBlocks[i]/numDisks;
        int remoteFunc = READ;
        MPI_Send(&remoteFunc, 1, MPI_INT, 0, 0, *(diskVector[vectorOfBlocks[i]%numDisks]));
        MPI_Send(&remoteBlockId, 1, MPI_INT, 0, 0, *(diskVector[vectorOfBlocks[i]%numDisks]));
        MPI_Recv(dataBock, sizeOfBlock, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, *(diskVector[vectorOfBlocks[i]%numDisks]), &status);

        memcpy(normalizedData+(i*sizeOfBlock), dataBock, sizeof(char)*sizeOfBlock);
    }

    memcpy(file, normalizedData, sizeFile);

    free(dataBock);
    free(normalizedData);
}
/*!
 * \brief DiskManager::saveTables Método para ir guardando los id del bloques libres en un archivo
 * Vamos a ver cuales son los sectores libres y esos los vamos a ir añadiendo.
 * Más tarde iremos escribiendo ese número de bloques libres (ID) en un fichero.
 */
void DiskManager::saveTables()
{

    std::string freeSectors = pwdLocal;
    freeSectors += "/sectoresLibres.dat";

    FILE* sectorsFile = fopen(freeSectors.c_str(), "wb");

    int numFreeBlocks = vectorOfFreeBlocksRAID.size();

    fwrite(&numFreeBlocks, sizeof(int), 1, sectorsFile);
    for(std::list<int>::iterator it = vectorOfFreeBlocksRAID.begin(); it != vectorOfFreeBlocksRAID.end(); ++it)
    {
        int tmpBlockID = *it;
        fwrite(&tmpBlockID ,sizeof(int), 1, sectorsFile);
    }
    fclose(sectorsFile);
}
/*!
 * \brief DiskManager::setNumBlocks Método para setear el número de bloques
 * \param newNumBlocks el nuevo numero de bloques
 * Se realiza el formateo con el nuevo número de bloques
 */
void DiskManager::setNumBlocks(int newNumBlocks)
{
    numBlocks = newNumBlocks;

    format(newNumBlocks*numDisks);
}
/*!
 * \brief DiskManager::removeFile Método para borrar un fichero
 * \param vectorOfBlocks los bloques donde está el fichero a eliminar
 * Llamamos al saveTables después de eliminar un fichero porque el número de bloques libres cambia (aumenta).
 */
void DiskManager::removeFile(std::vector<int> vectorOfBlocks)
{
    for(int block : vectorOfBlocks)
    {
        vectorOfFreeBlocksRAID.push_front(block);
    }
    spaceInRAID += (vectorOfBlocks.size()*sizeOfBlock);

    saveTables();
}
