/*
 * Authors: Mario Cavero y Arturo Lara
*/

#ifndef TERMINAL_H
#define TERMINAL_H

#include <vector>

#include "datatree.h"
#include "diskmanager.h"

/*!El número de bloques que tendrá nuestro disco*/
const int numBlocks = 32000;
/*! Un tipo enum para los comandos
     *  Esto nos va a servir para saber qué comando inserta el usuario por consola
     * en nuestra aplicación. Se pueden ir añadiendo según qué comandos queramos
     * ir implementando.
     */
enum command_e
{
    NO_COMMAND = 0,
    CD = 1,
    LS = 2,
    PWD = 3,
    MV = 4,
    CP = 5,
    MKDIR = 6,
    RMDIR = 7,
    RM = 8,
    LLS = 9,
    LCD = 10,
    LPWD = 11,
    UPLOAD = 12,
    END = 13,
    DOWNLOAD = 14
};
/*! \brief La estructura que define el comando
 La estructura para poder escribir y recibir los comandos en consola*/
typedef struct command_t{
    command_e type; /**< el tipo de comando que recibiremos> */
    std::vector<char*>* args;  /** < El comando en sí (el input)>*/

    //!Método para limpiar la consola de texto
    /*! Este método nos limpiará la consola y el comando*/
    void clean()
    {
        type = command_e::NO_COMMAND; /** <ponemos el comando como si no tuvieramos>*/
        args->clear(); /** <Vaciamos el input que tengamos> */
    }
}command_t;
/*! \brief La clase Terminal
 Esta clase será la que tenga todos los métodos que ejecuten los comandos
pasados por el usuario
*/
class Terminal{

private:
    DataTree* tree; /**< El árbol a utilizar> */
    DiskManager* raid; /** <El disk manager que controlará los discos> */
    bool exit=false; /** <para saber cuando acabamos> */

    /// \brief Método para no poder hacer ciertas funciones en un mismo directorio
    /// \param OriginDirectory el nodo origen
    /// \param destDirectory el nodo destino (que miraremos sea igual al origen)
    /// \return bool un booleano sobre si lo hemos encontrado o no
    bool findSubdirectoryDependency(node_t* OriginDirectory, node_t* destDirectory);

    /// \brief Método para encontrar la direccion actual de manera recursiva
    /// \param El nodo que se va a ir pasando para encontrar
    /// \return string con la direccion total encontrada
    std::string pwdRecursive(node_t* node);

    /// \brief Método para encontrar el path
    /// \param command
    /// \param actualDirectory
    /// \param index
    /// \param dirFlag
    /// \return node_t el último nodo (toda la path hasta él)
    node_t* findByPathRecursive(char* command, node_t* actualDirectory, int index, bool dirFlag);

    /// \brief Método que llama al método recursivo de encontrar la path según hayamos escrito "/" o no al final
    /// \param command el parametro pasado
    /// \return node_t el path encontrado
    node_t* findByPath(char* command);

    /// \brief Método para encontrar un directorio en el directorio
    /// \param directoryNode el nodo del que partimos
    /// \param DirectoryName el directorio a buscar
    /// \return node_t nos devuelve el nodo directorio encontrado
    node_t* findDirectoryAtDirectory(node_t* directoryNode, std::string DirectoryName);

    /// \brief Método para encontrar un fichero en un directorio
    /// \param directoryNode es el nodo directorio
    /// \param fileName es el nombre del fichero
    /// \return node_t es el nodo encontrado
    node_t* findFileAtDirectory(node_t* directoryNode, std::string fileName);

    /// \brief Método para encontrar un nodo en un directorio
    /// \param directoryNode es el nombre del directorio
    /// \param nodeName es el nombre del nodo a encontrar
    /// \return node_t es el nodo encontrado
    node_t* findNodeAtDirectory(node_t* directoryNode, std::string nodeName);

    /// \brief Método para copiar un directorio en el destino
    /// \param OriginDirectory es el Directorio origen
    /// \param destDirectory es el Directorio destino
    /// \param newNameDirectory es el nombre del directorio destino a poner
    void copyDirectoryRecursive(node_t* OriginDirectory, node_t* destDirectory, std::string newNameDirectory);

    /// \brief Método para copiar un directorio del Local
    /// \param destDirectory va a ser el directorio donde se copiará
    void copyLocalDirectoryRecursive(node_t* destDirectory);

    /// \brief Recibimos la salida del comando
    /// \param cmd es el comando del que queremos obtener la salida
    /// \return string es la salida del comando pasado.
    std::string getOutFromCommand(std::string cmd);

public:
    /// \brief El constructor de la clase
    Terminal();

    /// \brief Método que carga el árbol y lee/ejecuta comandos.
    void run();

    /// \brief Método que lee un comando
    /// \param comando es el comando a leer
    void readCommand(command_t* comando);

    /// \brief Método para ejecutar un comando
    /// \param comando es el comando a ejecutar
    void runCommand(command_t comando);

    /// \brief Método que según lo que le mandemos, nos dice el comando que es
    /// \param comando es el comando que le pasamos
    /// \return command_e es el tipo de comando que devuelve
    command_e getTypeOfCommand(char* comando);

    /// \brief Método que ejecuta el comando ls en nuestro árbol
    void ls();

    /// \brief Método que ejecuta el comando cd en nuestro árbol
    /// \param command es a donde vamos a hacer cd (todo el comando entero)
    void cd(command_t command);

    /// \brief Método que ejecuta el comando pwd en nuestro árbol
    void pwd();

    /// \brief Método que ejecuta el comando para crear un directorio en nuestro árbol
    /// \param command el comando entero para crear el directorio
    void mkdir(command_t command);

    /// \brief Método que ejecuta el comando de borrado un directorio
    /// \param command es el comando para borrar el directorio
    void rmdir(command_t command);

    /// \brief Método para borrar un fichero en el árbol
    /// \param aCommand es el comando entero para borrar
    void rm(command_t aCommand);

    /// \brief Método para subir al árbol
    /// \param aCommand es el comando entero que nos determinará qué se subirá
    void upload(command_t aCommand);

    /// \brief Método para cambiar de nombre un fichero/directorio en nuestro árbol
    /// \param aCommand es el comando entero para cambiar el nombre.
    void mv(command_t aCommand);
    void cp(command_t aCommand);
    void lls();
    void lcd(command_t aCommand);
    void lpwd();
    void download(command_t aCommand);
};

#endif // TERMINAL_H
