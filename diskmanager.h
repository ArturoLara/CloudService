#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <vector>
#include <list>
#include <string>
#include <sys/stat.h>
#include "/usr/include/mpi/mpi.h"

const off_t sizeOfBlock = 1024;/**<El tamaño de bloque >*/
const int numDisks = 4; /**<El número de discos a utilizar >*/
/*!
 * \brief The function_e enum es la que usaremos para saber si estamos escribiendo o leyendo en disco
 */
enum function_e
{
    WRITE = 1,
    READ = 2
};
/*!
 * \brief The DiskManager class será la controladora de los discos y los bloques
 * Con ella veremos como ir guardando y quitando ficheros de los discos.
 */
class DiskManager
{
public:
    /// \brief El constructor de la clase controladora
    /// \param pwd será la direccion total con la que sabremos donde poner los bloques
    /// \param numBlocks son el numero de bloques totales
    DiskManager(std::string pwd, int numBlocks);

    /// \brief Método para escribir un fichero en disco
    /// \param file es el fichero a escribir
    /// \param sizeFile es el tamaño del fichero a escribir
    /// \return usedBlocks nos devolverá el número de bloques usados
    std::vector<int> writeFile(char* file, off_t sizeFile);

    /// \brief Método para leer un archivo del disco
    /// \param file será el fichero a leer
    /// \param sizeFile será el tamaño del fichero a leer
    /// \param vectorOfBlocks serán los bloques en los que está ese fichero.
    void readFile(char* file, off_t sizeFile, std::vector<int> vectorOfBlocks);

    /// \brief Método para eliminar un fichero del disco
    /// \param vectorOfBlocks son los bloques donde está el fichero
    void removeFile(std::vector<int> vectorOfBlocks);

    /// \brief Método para especificar el numero de bloques por disco
    /// \param newNumBlocks son la cantidad nueva de numeros de bloque
    void setNumBlocks(int newNumBlocks);

    /// \brief Método llamado en el seteo de bloques y más, para escribir los bloques en disco
    /// \param numBlocks serán los bloques a escribir
    void format(int numBlocks);

private:
    std::string pwdLocal; /**<Es la direccion total local> */
    int numBlocks; /**<El número> */
    off_t spaceInRAID; /**<El espacio que hay en disco>*/
    std::list<int> vectorOfFreeBlocksRAID;/**<Lista con una lista de los bloques libres por disco>*/
    std::vector<MPI_Comm*> diskVector;/**<Vector de comunnicadores de discos para irse comunicando con los discos>*/

    ///\brief Método para guardar los id de los bloques libres en un archivo (sectoresLibres.dat)
    void saveTables();
};

#endif // DISKMANAGER_H
