/*
 * Authors: Mario Cavero y Arturo Lara
*/

#include "datatree.h"

#include <algorithm>
#include <iostream>
#include <cstring>
/*!
El constructor del árbol. Vamos a recibir pwd como parámetro.
Creamos el nodo root, y actualizamos los distintos parámetros
*/
DataTree::DataTree(std::string pwd)
{
    node_t* aRootNode = new node_t(INITIAL_NODE_ID, NULL, "root", INITIAL_DEEP_LEVEL, true, SIZE_OF_DIRECTORY);
    rootNode = aRootNode;
    actualDirectoryNode = aRootNode;
    nodeCount = 1;
    lastIdNode = INITIAL_NODE_ID;
    pwdLocal = pwd;
}
/*!
 \brief DataTree::saveTree
 saveTree nos va a guardar el árbol. Para ello, llamaremos al
 método privado de guardar árbol de forma recursiva.
 Tendremos nuestro path acabado en /arbol.dat que será donde lo guardaremos
 Abrimos (o creamos en caso de no existir), ese archivo, y escribiremos
 las cabeceras (la cantidad de nodos y el id del último nodo).
 Si el nodo root tiene hijos, procederemos a guardar el árbol de forma recursiva.
 */
void DataTree::saveTree()
{
    FILE* file;
    std::string pathToBackup = pwdLocal;
    pathToBackup += "/arbol.dat";
    file = fopen(pathToBackup.c_str(), "wb");
    fwrite(&nodeCount, sizeof(unsigned int), 1, file);
    fwrite(&lastIdNode, sizeof(unsigned int), 1, file);

    if(!rootNode->childNodes.empty())
    {
        for(node_t* childNode : rootNode->childNodes)
        {
            saveTreeRecursive(childNode, file);
        }
    }

    fclose(file);
}
/*!
 * \brief DataTree::saveTreeRecursive
 * \param node El nodo que le vamos a ir pasando de forma recursiva
 * \param file
 * Método privado para guardar el árbol de manera recursiva
 * Escribimos los atributos del nodo en el fichero
 * Por cada bloque, guardamos el primer y segundo elemento (disco y bloque)
 * Si el árbol sigue sin estar vacío, volvemos a llamar a la recursiva
 */
void DataTree::saveTreeRecursive(node_t* node, FILE* file)
{
    char name[25];
    int size = node->vectorOfBlocksId.size();
    strcpy(name, node->nameNode.c_str());
    name[25] = '\0';
    fwrite(name,sizeof(char),sizeof(name), file);
    fwrite(&(node->deepLevel), sizeof(unsigned int), 1, file);
    fwrite(&(node->directoryFlag), sizeof(bool), 1, file);
    fwrite(&(node->lastChange), sizeof(time_t), 1, file);
    fwrite(&(node->size), sizeof(off_t), 1, file);
    fwrite(&size, sizeof(int), 1, file);

    for(int i = 0; i < node->vectorOfBlocksId.size(); i++)
    {
        fwrite(&(node->vectorOfBlocksId[i].first), sizeof(int), 1, file);
        fwrite(&(node->vectorOfBlocksId[i].second), sizeof(int), 1, file);
    }

    if(!node->childNodes.empty())
    {
        for(node_t* childNode : node->childNodes)
        {
            saveTreeRecursive(childNode, file);
        }
    }
}
/*!
 * \brief DataTree::loadTree
 * Método para cargar el árbol.
 * Abrimos (o creamos si no existe) árbol.dat
 * vamos a leer el numero de nodos y el id del último nodo
 * Como ya lo hemos leído, vamos a cargar de manera recursiva
 * desde el nodo root (hasta los hijos si tiene).
 */
void DataTree::loadTree()
{
    FILE* file;
    unsigned int lastID, count;

    file = fopen("arbol.dat", "rb");
    if(file != NULL)
    {
        fread(&count, sizeof(unsigned int), 1, file);
        fread(&lastID, sizeof(unsigned int), 1, file);

        loadTreeRecursive(rootNode, file, count);

        lastIdNode = lastID;

        fclose(file);
    }
    else
    {
        std::cout << "Not tree backup found, nothing loaded" << std::endl;
    }

}
/*!
 * \brief DataTree::loadTreeRecursive
 * \param node
 * \param file
 * \param totalNodes
 * \return newNode
 * Leemos del fichero los distintos atributos del nodo
 * por cada bloque, leemos el id del disco y el bloque del disco, y
 * eso se lo asignamos al par (id disco y bloque de disco).
 * Mientras el nodo pasado por parámetro tenga hijos (su profundidad sea menor),
 * vamos a ir cargando el nuevo nodo.
 */
node_t* DataTree::loadTreeRecursive(node_t* node, FILE* file, unsigned int totalNodes)
{
    if(nodeCount < totalNodes)
    {
        char name[25];
        unsigned int depth;
        bool flagDir;
        time_t lastChange;
        off_t size;
        int numBlocks, blockId, diskId;
        std::vector<std::pair<int,int>> vectorBlockId;

        fread(&name, sizeof(char), sizeof(name), file);
        fread(&depth, sizeof(unsigned int), 1, file);
        fread(&flagDir, sizeof(bool), 1, file);
        fread(&lastChange, sizeof(time_t), 1, file);
        fread(&size, sizeof(off_t), 1, file);
        fread(&numBlocks, sizeof(int), 1, file);

        for(int i = 0; i < numBlocks; i++)
        {
            fread(&diskId, sizeof(int), 1, file);
            fread(&blockId, sizeof(int), 1, file);
            std::pair<int,int> tempPair(diskId, blockId);
            vectorBlockId.push_back(tempPair);
        }

        node_t* newNode = new node_t(nodeCount, node, name, depth, flagDir, size);
        newNode->vectorOfBlocksId = vectorBlockId;
        newNode->lastChange = lastChange;
        nodeCount++;

        while(node->deepLevel < newNode->deepLevel)
        {
            newNode->fatherNode = node;
            node->childNodes.push_back(newNode);
            newNode = loadTreeRecursive(newNode, file, totalNodes);
            if(newNode == NULL)
            {
                break;
            }
        }
        return newNode;
    }
    else
    {
        return NULL;
    }
}
/*!
 * \brief DataTree::addNode
 * \param aFatherNode
 * \param aNameNode
 * \param aDirectory
 * \param aSize
 * \param vectorOfBlocksId
 * \return newNode el nodo añadido
 * creamos un nuevo nodo con los parámetros pasados, con un nivel más de profundidad.
 * Si no es un directorio, tiene un par (disco, bloque).Así que se lo asignamos
 * Y actualizamos los valores.
 */
node_t* DataTree::addNode(node_t* aFatherNode, std::string aNameNode, bool aDirectory, off_t aSize, std::vector<std::pair<int, int>> vectorOfBlocksId)
{
    unsigned int newDeepLevel = aFatherNode->deepLevel + 1;
    node_t* newNode = new node_t(nodeCount, aFatherNode, aNameNode, newDeepLevel, aDirectory, aSize);

    if(!aDirectory)
    {
        newNode->vectorOfBlocksId = vectorOfBlocksId;
    }

    aFatherNode->childNodes.push_back(newNode);
    lastIdNode = nodeCount;
    nodeCount += 1;
    return newNode;
}
/*!
 * \brief DataTree::findNodeRecursive
 * \param aIdNode
 * \param rootNode
 * \return foundNode
 * Para encontrar el nodo de forma recursiva, miramos si es el nodo root.
 * Si no lo es, vamos a ir mirando por los nodos hijos hasta ver si ese
 * es el nodo que buscamos.
 */
node_t* DataTree::findNodeRecursive(unsigned int aIdNode, node_t* rootNode)
{
    node_t* foundNode = NULL;

    if(rootNode->id == aIdNode)
    {
        foundNode = rootNode;
    }
    else
    {
        for(node_t* node : rootNode->childNodes)
        {
            foundNode = findNodeRecursive(aIdNode, node);
            if(foundNode != NULL)
            {
                break;
            }
        }
    }

    return foundNode;
}
/*!
 * \brief DataTree::findNode
 * \param aIdNode
 * \return findNodeRecursive(aIdNode, rootNode);
 * llamamos al método recursivo para encontrar el nodo
 */
node_t* DataTree::findNode(unsigned int aIdNode)
{
    return findNodeRecursive(aIdNode, rootNode);
}
/*!
 * \brief DataTree::updateNode
 * \param aIdNode
 * \param aNameNode
 * \param aSize
 * Metodo para updatear un nodo.
 * Primero encontramos el nodo que queremos updatear con su id,
 * y de ahí updateamos los valores.
 */
void DataTree::updateNode(unsigned int aIdNode, std::string aNameNode, off_t aSize)
{
    node_t* nodeToModify = findNode(aIdNode);
    nodeToModify->nameNode = aNameNode;
    nodeToModify->size = aSize;
    time(&(nodeToModify->lastChange));
}
/*!
 * \brief DataTree::removeNode
 * \param nodeToRemove
 * Para eliminar el nodo, encontramos el nodo que queremos eliminar
 * y una vez lo encontramos, lo eliminamos.
 */

void DataTree::removeNode(node_t* nodeToRemove)
{
    auto it = std::find(actualDirectoryNode->childNodes.begin(), actualDirectoryNode->childNodes.end(), nodeToRemove);
    if (it != actualDirectoryNode->childNodes.end())
    {
      actualDirectoryNode->childNodes.erase(it);
    }
    delete(nodeToRemove);
}
