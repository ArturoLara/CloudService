/*
 * Authors: Mario Cavero y Arturo Lara
*/

#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>

/*! \brief La estructura principal del Nodo
 La estructura básica de nodos que utilizará nuestro árbol
Tiene distintos parámetros.
 * */
struct node_t
{
    unsigned int id; /**< El id del nodo> */
    node_t* fatherNode; /**< El puntero al nodo Padre del nodo actual> */
    std::string nameNode; /**< El nombre del nodo> */
    std::vector<node_t*> childNodes; /**< El vector con los hijos del nodo actual> */
    unsigned int deepLevel; /**< La profundidad del nodo > */
    bool directoryFlag; /**< Booleano para saber si el nodo es un directorio o un archivo> */
    off_t size; /**< El tamaño del nodo> */
    time_t lastChange; /**< La última modificacion realizada en ese nodo> */
    std::vector<int> vectorOfBlocksId; /**< Un vector con el bloque donde se encuentra el nodo> */

    //! El constructor del nodo
    /*! El constructor tomará los valores de la estructura por parámetros y los inicializará.  */
    node_t(unsigned int id, node_t* fatherNode, std::string nameNode, unsigned int actualDeepLevel, bool directory, off_t size)
    {
        this->id = id;
        this->fatherNode = fatherNode;
        this->nameNode = nameNode;
        this->deepLevel = actualDeepLevel;
        this->directoryFlag = directory;
        this->size = size;
        time(&(this->lastChange));
    }

};

#endif // NODE_H
