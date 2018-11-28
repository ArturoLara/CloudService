/*
 * Authors: Mario Cavero y Arturo Lara
*/

#ifndef DATATREE_H
#define DATATREE_H

#include <string>
#include <vector>
#include <sys/stat.h>

#include "node.h"

const unsigned int INITIAL_NODE_ID = 0; /**< El id del nodo inicial> */
const unsigned int INITIAL_DEEP_LEVEL = 0; /**< La profundidad incial> */
const off_t SIZE_OF_DIRECTORY = 4096; /**< El tamaño del direcctorio> */
const int SIZE_OF_BLOCK = 46; /**< El tamaño del bloque> */
/*! \brief La clase DataTree es la clase del árbol */
/*!
    La clase que vamos a utilizar para nuestro árbol.
    Tendrá distintos métodos que utilicen los nodos, añadirlos y demás.

*/
class DataTree
{
public:
    /// \brief El constructor del árbol
    /// \param pwd es la direccion princiapl
    DataTree(std::string pwd);

    /// \brief Método para añadir un nodo
    /// \param aFatherNode El puntero al padre
    /// \param aNameNode El nombre del nodo a añadir
    /// \param aDirectory Para saber si es directorio o no.
    /// \param aSize nos indica el tamaño del nodo a añadir
    /// \param vectorOfBlocksId son el par de disco y bloque del nodo
    /// \return El nodo a añadir
    node_t* addNode(node_t* aFatherNode, std::string aNameNode, bool aDirectory, off_t aSize, std::vector<std::pair<int, int>> vectorOfBlocksId = std::vector<std::pair<int, int>>());

    /// \brief Método para encontrar un nodo
    /// \param aIdNode Es el nodo a encontrar
    /// \return el nodo encontrado
    node_t* findNode(unsigned int aIdNode);

    /// \brief Método para updatear un nodo
    /// \param aIdNode es el nodo a updatear
    /// \param aNameNode es el nombre a updatear
    /// \param size es el parametro a updatear
    void updateNode(unsigned int aIdNode, std::string aNameNode, off_t size);

    /// \brief Método para eliminar el nodo
    /// \param aIdNode El id del nodo a eliminar
    void removeNode(node_t* aIdNode);

    /// \brief Método para saber el directorio actual del nodo
    /// \return el directorio del nodo
    inline node_t* getActualDirectoryNode(){ return this->actualDirectoryNode; }

    /// \brief Método para setear el directorio del nodo
    /// \param El nodo del directorio
    inline void setActualDirectoryNode(node_t* node){ this->actualDirectoryNode = node; }

    /// \brief Método para guardar el árbol
    void saveTree();

    /// \brief Método para cargar el árbol
    void loadTree();
private:
    std::string pwdLocal; /**< La direccion local > */
    node_t* rootNode; /**< Puntero al nodo Root> */
    node_t* actualDirectoryNode; /**< Puntero al nodo actual(directorio)> */
    unsigned int nodeCount; /**< Cantidad de nodos> */
    unsigned int lastIdNode; /**< Id del último nodo> */

    /// \brief Método que encuentra un nodo de forma recursiva
    /// \param aIdNode el id del nodo
    /// \param rootNode el nodo raíz
    /// \return el nodo encontrado
    node_t* findNodeRecursive(unsigned int aIdNode, node_t* rootNode);

    /// \brief Método para guardar el árbol de forma recursiva
    /// \param node es el nodo desde el cual se quiere guardar el árbol
    /// \param file es el fichero donde se va a guardar el árbol
    void saveTreeRecursive(node_t* node, FILE* file);

    /// \brief Método para cargar el árbol de forma recursiva
    /// \param el nodo desde el cual se quiere cargar el árbol
    /// \parm file el fichero de donde lo vamos a cargar
    /// \param totalNodes el número total de nodos en el árbol.
    /// \return puntero al último nodo.
    node_t* loadTreeRecursive(node_t* node, FILE* file, unsigned int totalNodes);
};

#endif // DATATREE_H
