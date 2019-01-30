/*
 * Authors: Mario Cavero y Arturo Lara
*/

#include <string>
#include "/usr/include/mpi/mpi.h"

const off_t sizeOfBlock = 1024;
/*!
 * \brief function_e es de tipo enum para saber el modo del disco.
 * */
enum function_e
{
    WRITE = 1,
    READ = 2
};

/*!
 * \brief writeBlock Función para escribir en un bloque con MPI
 * \param idDisk
 * writeBlock va a comunicarse con el padre, y recibir donde tiene que escribir.
 * buscará el bloque con su id, y lo escribirá
 */
void writeBlock(int idDisk)
{
    int blockID;

    MPI_Status status;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm parent;
    MPI_Comm_get_parent(&parent);

    char* block = (char*)malloc(sizeof(char)*sizeOfBlock);

    MPI_Recv(&blockID, 1, MPI_INT, 0,MPI_ANY_TAG, parent,&status );
    MPI_Recv(block, sizeOfBlock, MPI_CHAR, 0,MPI_ANY_TAG, parent,&status );

    std::string diskName = "/home/ubuntu/MarioCavero_ArturoLara/disco";
    diskName += std::to_string(idDisk);
    diskName += ".dat";

    FILE* diskFile = fopen(diskName.c_str(), "ab");
    if(diskFile != NULL)
    {
        fseek(diskFile, (blockID)*sizeOfBlock, SEEK_SET);
        fwrite(block ,sizeof(char), sizeOfBlock, diskFile);
        fclose(diskFile);
    }
    free(block);
}
/*!
 * \brief readBlock Es una función muy parecida a write block
 * \param idDisk
 * Va a recibir el id del disco y al comunicarse con el padre
 * va a leer del disco, el bloque que se le indica
 */
void readBlock(int idDisk)
{
    MPI_Status status;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm parent;
    MPI_Comm_get_parent(&parent);

    char* block = (char*)malloc(sizeof(char)*sizeOfBlock);
    int blockID;

    MPI_Recv(&blockID, 1, MPI_INT, 0,MPI_ANY_TAG, parent,&status );

    std::string diskName = "/home/ubuntu/MarioCavero_ArturoLara/disco";
    diskName += std::to_string(idDisk);
    diskName += ".dat";

    FILE* diskFile = fopen(diskName.c_str(), "rb");
    if(diskFile != NULL)
    {
        fseek(diskFile, (blockID)*sizeOfBlock, SEEK_SET);
        fread(block ,sizeof(char), sizeOfBlock, diskFile);

        MPI_Send(block, sizeOfBlock, MPI_CHAR, 0, 0, parent);

        fclose(diskFile);
    }
    else
    {
        std::cout << "ERROR: Disco " << idDisk << " perdido" << std::endl;
    }
    free(block);
}

int main(int argc, char** argv)
{
    MPI_Comm parent;
    MPI_Status status;

    MPI_Init(&argc, &argv);

    int rank, idDisk;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm_get_parent(&parent);

    MPI_Recv(&idDisk, 1, MPI_INT, 0, MPI_ANY_TAG, parent, &status);

    std::string diskName = "/home/ubuntu/MarioCavero_ArturoLara/disco";
    diskName += std::to_string(idDisk);
    diskName += ".dat";

    FILE* diskFile = fopen(diskName.c_str(), "rb");

    if(diskFile == NULL)
    {
        std::cout << "No se ha encontrado el disco " << idDisk << ", se creará un nuevo disco" << std::endl;
        FILE* tmp = fopen(diskName.c_str(), "wb");
        fclose(tmp);
    }
    else
    {
        fclose(diskFile);
    }

    int func = 0;

    while(true)
    {
        MPI_Recv(&func, 1, MPI_INT, 0,MPI_ANY_TAG, parent,&status );

        switch (func) {
        case WRITE:
            writeBlock(idDisk);
            break;
        case READ:
            readBlock(idDisk);
            break;
        default:
            std::cout << "Peticion a disco no valida" << std::endl;
            break;
        }
    }
    return 0;
}
