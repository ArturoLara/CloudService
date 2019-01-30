/*
 * Authors: Mario Cavero y Arturo Lara
*/

#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sstream>
/*!
 * \brief Terminal::Terminal
 * El constructor de la terminal
 * Nos inicializa el Árbol y el Controlador del disco (DiskManager)
 */
Terminal::Terminal()
{
    tree = new DataTree(getOutFromCommand("pwd | tr -d \'\n\'"));
    raid = new DiskManager(getOutFromCommand("pwd | tr -d \'\n\'"), numBlocks);
}
/*!
 * \brief Terminal::cd
 * \param aCommand la ruta a la que vamos a ir
 * Método que sirve para movernos a un directorio especificado
 * Abordamos las causas de "cd ..", "cd nombre", y cd pasado con /
 */
void Terminal::cd(command_t aCommand){
    if(aCommand.args->size() == 2)
    {
        bool completeFlag = false;
        if(!strncmp("..", aCommand.args->at(0), 2))
        {
            node_t* node = this->tree->getActualDirectoryNode()->fatherNode;
            if(node != NULL)
            {
                this->tree->setActualDirectoryNode(node);
                completeFlag = true;
            }
        }
        else if(!strncmp("/", aCommand.args->at(0), 1))
        {
            tree->setActualDirectoryNode(tree->findNode(0));
            completeFlag = true;
        }
        else
        {
            for(node_t* node : this->tree->getActualDirectoryNode()->childNodes)
            {
                if(strlen(aCommand.args->at(0)) == node->nameNode.size())
                {
                    if(!strncmp(aCommand.args->at(0) , node->nameNode.c_str(), node->nameNode.size()))
                    {
                        if(node->directoryFlag)
                        {
                            this->tree->setActualDirectoryNode(node);
                            completeFlag = true;
                            break;
                        }
                    }
                }
            }
            if(!completeFlag)
            {
                std::cout << "It's not a directory" << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }
}
/*!
 * \brief Terminal::ls
 * Método que nos lista todos los nodos que hay en donde nos encontremos
 */

void Terminal::ls(){
    std::cout << std::endl;
    for(node_t* node : this->tree->getActualDirectoryNode()->childNodes)
    {
        std::cout << node->nameNode << " ";
        if(node->directoryFlag)
        {
            std::cout << "(directory) ";
        }
        else
        {
            std::cout << "(file) ";
        }
        tm* timeinfo = localtime (&node->lastChange);
        std::cout << node->size << "Bytes " << asctime(timeinfo) << std::endl;
    }
}
/*!
 * \brief Terminal::findDirectoryAtDirectory
 * \param directoryNode es el nodo base
 * \param DirectoryName es el nombre del directorio a encontrar
 * \return nodeToReturn nos va a devolver el nodo encontrado
 * Este método nos va a encontrar un directorio en el directorio actual
 */
node_t* Terminal::findDirectoryAtDirectory(node_t* directoryNode, std::string DirectoryName)
{
    node_t* nodeToReturn = NULL;
    for(int index = 0; index < directoryNode->childNodes.size(); index++)
    {
        node_t* node = directoryNode->childNodes.at(index);
        if(DirectoryName == node->nameNode)
        {
            if(node->directoryFlag)
            {
                nodeToReturn = node;
                break;
            }
        }
    }
    return nodeToReturn;
}
/*!
 * \brief Terminal::findFileAtDirectory
 * \param directoryNode el nodo base
 * \param fileName el nombre del fichero a buscar
 * \return nodeToReturn es el fichero encontrado
 * Este método nos encuentra un fichero en un directorio.
 * Siempre vamos a buscar por el número total de hijos en el directorio.
 */
node_t* Terminal::findFileAtDirectory(node_t* directoryNode, std::string fileName)
{
    node_t* nodeToReturn = NULL;
    for(int index = 0; index < directoryNode->childNodes.size(); index++)
    {
        node_t* node = directoryNode->childNodes.at(index);
        if(fileName == node->nameNode)
        {
            if(!node->directoryFlag)
            {
                nodeToReturn = node;
                break;
            }
        }
    }
    return nodeToReturn;
}
/*!
 * \brief Terminal::findNodeAtDirectory
 * \param directoryNode es el nodo base
 * \param nodeName es el nombre del nodo
 * \return nodeToReturn es el nodo encontrado
 * Este método nos va a buscar un nodo en un directorio, y si existe
 *  nos lo va a devolver
 * Siempre vamos a mirar en todos los hijos del directorio en el que estamos.
 */
node_t* Terminal::findNodeAtDirectory(node_t* directoryNode, std::string nodeName)
{
    node_t* nodeToReturn = NULL;
    for(int index = 0; index < directoryNode->childNodes.size(); index++)
    {
        node_t* node = directoryNode->childNodes.at(index);
        if(nodeName == node->nameNode)
        {
            nodeToReturn = node;
            break;
        }
    }
    return nodeToReturn;
}
/*!
 * \brief Terminal::copyDirectoryRecursive
 * \param OriginDirectory es el nodo origen
 * \param destDirectory es el nodo destino
 * \param newNameDirectory es el nuevo nombre
 * Este método nos va a copiar un directorio (con todos sus hijos, de manera recursiva)
 * en el directorio especificado.
 * Como estamos copiando, significa que necesitamos espacio y escribirlo en disco
 * Con lo que iremos escribiendo el nodo que vayamos copiando
 */
void Terminal::copyDirectoryRecursive(node_t* OriginDirectory, node_t* destDirectory, std::string newNameDirectory)
{
    std::vector<int> fileBlocksId;
    if(!OriginDirectory->directoryFlag)
    {

        char* dataFile = (char*)malloc(sizeof(char)*OriginDirectory->size);

        raid->readFile(dataFile, OriginDirectory->size, OriginDirectory->vectorOfBlocksId);
        fileBlocksId = raid->writeFile(dataFile, OriginDirectory->size);
        free(dataFile);
    }
    destDirectory = tree->addNode(destDirectory, newNameDirectory, OriginDirectory->directoryFlag, OriginDirectory->size, fileBlocksId);
    for(node_t* node : OriginDirectory->childNodes)
    {
        copyDirectoryRecursive(node, destDirectory, node->nameNode);
    }
}
/*!
 * \brief Terminal::copyLocalDirectoryRecursive Método parecido al cp pero del Local
 * \param destDirectory es el directorio destino
 * Mientras vaya habiendo directorios, vamos a ir copiándolos y añadiéndolos al árbol
 */
void Terminal::copyLocalDirectoryRecursive(node_t* destDirectory)
{
    std::string token;
    std::string localItemList = getOutFromCommand("ls | sort");
    std::stringstream streamString(localItemList);

    while(std::getline(streamString, token, '\n'))
    {
        struct stat buffer;
        stat(token.c_str(), &buffer);
        if(S_ISREG(buffer.st_mode))
        {
            FILE* tempFile = fopen(token.c_str(), "rb");
            char* dataFile = (char*)malloc(sizeof(char)*buffer.st_size);

            fread(dataFile, sizeof(char), buffer.st_size, tempFile);

            std::vector<int> fileBlocksId = raid->writeFile(dataFile, buffer.st_size);
            tree->addNode(tree->getActualDirectoryNode(), token, false, buffer.st_size, fileBlocksId);

            fclose(tempFile);
            free(dataFile);
        }
        else
        {
            node_t* nextDir = tree->addNode(destDirectory, token, true, SIZE_OF_DIRECTORY);
            chdir(token.c_str());
            copyLocalDirectoryRecursive(nextDir);
            chdir("..");
        }
    }
}
/*!
 * \brief Terminal::getOutFromCommand Método para recibir la salida del comando (output)
 * \param cmd es el comando del que queremos obtener la salida
 * \return data es el output
 * Vamos a ir añadiendo al buffer cada "parámetro" del comando
 */
std::string Terminal::getOutFromCommand(std::string cmd)
{
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");
    stream = popen(cmd.c_str(), "r");

    if(stream)
    {
        while(!feof(stream))
        {
            if(fgets(buffer, max_buffer, stream) != NULL)
            {
                data.append(buffer);
            }
        }
        pclose(stream);
    }
    return data;
}
/*!
 * \brief Terminal::findSubdirectoryDependency Método para prueba de errores
 * \param OriginDirectory es el directorio origen
 * \param destDirectory es el directorio destino
 * \return dependency es el valor de si se puede o no
 * Este método es un control de errores para poder o no hacer ciertas funciones
 *  en un mismo directorio
 */
bool Terminal::findSubdirectoryDependency(node_t* OriginDirectory, node_t* destDirectory)
{
    bool dependency = false;

    if(OriginDirectory->id == destDirectory->id)
    {
        dependency = true;
    }

    for(node_t* node : destDirectory->childNodes)
    {
        dependency = findSubdirectoryDependency(OriginDirectory, node);
    }

    return dependency;
}
/*!
 * \brief Terminal::findByPath Método para encontrar el path
 * \param command es el parámetro pasado
 * \return funcion recursiva
 * Método que llama al método recursivo de encontrar la path según hayamos escrito "/"
 *  o no al final
 */
node_t* Terminal::findByPath(char *command){
    bool dirFlag = false;
    if(!strncmp(&(command[strlen(command)-1]), "/", 1))
    {
        dirFlag = true;
    }
    if(!strncmp(&(command[0]), "/", 1))
    {
        return findByPathRecursive(command, tree->findNode(0), 1, dirFlag);
    }
    else
    {
        return findByPathRecursive(command, tree->getActualDirectoryNode(), 0, dirFlag);
    }

}
/*!
 * \brief Terminal::findByPathRecursive El verdadero  método para encontrar el path
 * \param path es el parametro
 * \param actualDirectory el directorio en el que nos encontramos en ese momento
 * \param index para saber si estamos en "/"
 * \param dirFlag para saber si es un directorio
 * \return un nodo, el target o el actual, el último nodo (toda la path hasta él)
 * Con este método vamos a ir comprobando si donde estamos es donde queremos llegar
 * Si no lo es, vamos a ir llamándo a la función recursiva y vamos a
 * ir actualizando el path cada vez.
 */
node_t* Terminal::findByPathRecursive(char *path, node_t* actualDirectory, int index, bool dirFlag){


    char* actualPath=strtok(path+index, "/\n");
    char* nextPath = strtok(NULL , "/");

    std::string nextDir(actualPath);

    node_t* targetDirectory = findDirectoryAtDirectory(actualDirectory, nextDir);

    index += strlen(actualPath)+1;

    if(targetDirectory != NULL)
    {
        if(nextPath != NULL)
        {
            return findByPathRecursive(path, targetDirectory, index, dirFlag);
        }
        else
        {
            if(dirFlag)
            {
                return targetDirectory;
            }
            else
            {
                return actualDirectory;
            }
        }
    }
    else
    {
        if(nextPath != NULL)
        {
            return NULL;
        }
        else
        {
            if(dirFlag)
            {
                return NULL;
            }
            else
            {
                return actualDirectory;
            }
        }
    }
}
/*!
 * \brief Terminal::pwdRecursive Método para sacar la dirección actual recursivamente
 * \param aNode el nodo que vamos a ir pasando para encontrar la dirección
 * \return directory es la dirección total.
 * Con este método vamos a ir añadiendo una "/" cada vez que estemos en un dir
 * De esta manera, podremos encontrar la path completa "dirA/dirB/dirC"
 */
std::string Terminal::pwdRecursive(node_t* aNode){

    std::string directory;
    std::string name = aNode->nameNode;
    if(aNode->fatherNode != NULL)
    {
        directory = pwdRecursive(aNode->fatherNode);
    }
    directory += "/";
    directory += name;
    return directory;

}
/*!
 * \brief Terminal::pwd Método que saca la dirección actual
 * Para ello, llamaremos al pwdRecursive
 */
void Terminal::pwd(){
    std::cout << std::endl << pwdRecursive(this->tree->getActualDirectoryNode()) << std::endl;
}

/*!
 * \brief Terminal::mkdir Método que ejecuta el comando para crear un directorio en nuestro árbol
 * \param aCommand el comando entero para crear el directorio
 * Lo primero que haremos será controlar si acaba en "/" o en ".", para poder crear el dir.
 */
void Terminal::mkdir(command_t aCommand){
    if(aCommand.args->size() == 2)
    {
        std::string nameOfDir(aCommand.args->at(0));
        if(std::string::npos == nameOfDir.find("/") && std::string::npos == nameOfDir.find("."))
        {
            if(findDirectoryAtDirectory(tree->getActualDirectoryNode(), nameOfDir) == NULL)
            {
                tree->addNode(tree->getActualDirectoryNode(), nameOfDir, true, SIZE_OF_DIRECTORY);
            }
            else
            {
                std::cout << "Error: You can not use the same name as other directory" << std::endl;
            }
        }
        else
        {
            std::cout << "Error: You can not use special characters" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }

}
/*!
 * \brief Terminal::rmdir Método que ejecuta el comando de borrado un directorio
 * \param aCommand es el comando para borrar el directorio
 * Primero tenemos que ver si estamos en ese directorio
 * Una vez sabemos cual es el directorio, tenemos que ver si está vacío, o si es un fichero.
 */
void Terminal::rmdir(command_t aCommand){
    if(aCommand.args->size() == 2)
    {
        std::string dirToRemove(aCommand.args->at(0));

        node_t* nodeToRemove = NULL;
        int index;
        for(index = 0; index < tree->getActualDirectoryNode()->childNodes.size(); index++)
        {
            node_t* node = tree->getActualDirectoryNode()->childNodes.at(index);
            if(dirToRemove == node->nameNode)
            {
                if(node->directoryFlag)
                {
                    nodeToRemove = node;
                }
                break;
            }
        }

        if( nodeToRemove != NULL)
        {
            if(nodeToRemove->childNodes.size() > 0)
            {
                std::cout << "The directory is not empty: Can't be removed" << std::endl;
            }
            else
            {
                tree->removeNode(nodeToRemove);
            }
        }
        else
        {
            std::cout << "This directory doesn't exist or is a file" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }
}
/*!
 * \brief Terminal::rm Método para borrar un fichero en el árbol
 * \param aCommand es el comando entero para borrar
 * Tenemos que buscar el fichero a borrar, y de ahí, lo borraremos del disco también.
 */
void Terminal::rm(command_t aCommand)
{
    if(aCommand.args->size() == 2)
    {
        std::string fileToRemove(aCommand.args->at(0));
        node_t* nodeToRemove = NULL;

        nodeToRemove = findFileAtDirectory(tree->getActualDirectoryNode(), fileToRemove);

        if( nodeToRemove != NULL)
        {
            if(!nodeToRemove->directoryFlag)
            {
                raid->removeFile(nodeToRemove->vectorOfBlocksId);
            }
            tree->removeNode(nodeToRemove);
        }
        else
        {
            std::cout << "This file doesn't exist" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }
}
/*!
 * \brief Terminal::upload Métdo para subir al árbol
 * \param aCommand el comando entero que nos determinará qué se subirá
 * Si donde buscamos no existe ese nodo, procedemos a subirlo.
 * Para ello también tendremos que escribirlo en el disco.
 * Si es un directorio, como consecuencia tendremos que ir copiando todo.
 */
void Terminal::upload(command_t aCommand)
{
    if(aCommand.args->size() == 2)
    {
        std::string localNameToUpload(aCommand.args->at(0));

        struct stat buffer;
        if(findNodeAtDirectory(tree->getActualDirectoryNode(), localNameToUpload) == NULL)
        {
            if(stat(aCommand.args->at(0), &buffer) >= 0)
            {
                if(S_ISREG(buffer.st_mode))
                {
                    FILE* tempFile = fopen(aCommand.args->at(0), "rb");
                    char* dataFile = (char*)malloc(sizeof(char)*buffer.st_size);

                    fread(dataFile, sizeof(char), buffer.st_size, tempFile);

                    std::vector<int> fileBlocksId = raid->writeFile(dataFile, buffer.st_size);
                    if(fileBlocksId.size() > 0)
                        tree->addNode(tree->getActualDirectoryNode(), localNameToUpload, false, buffer.st_size, fileBlocksId);

                    fclose(tempFile);
                    free(dataFile);
                }
                else
                {
                    node_t* nextDir = tree->addNode(tree->getActualDirectoryNode(), localNameToUpload, true, SIZE_OF_DIRECTORY);
                    chdir(aCommand.args->at(0));
                    copyLocalDirectoryRecursive(nextDir);
                    chdir("..");
                }
            }
            else
            {
                std::cout << "This file or directory doesn't exist" << std::endl;
            }
        }
        else
        {
            std::cout << "Error: You can not use same name for 2 files or directories" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }
}
/*!
 * \brief Terminal::download Método para descargar un archivo (no directorio)
 * \param aCommand Lo que le pasamos por comando para el archivo a descargar
 * Buscamos el fichero en el árbol, y si lo encontramos y NO es un dir,
 * procedemos a descargarlo (leyéndolo del disco).
 */
void Terminal::download(command_t aCommand)
{
    if(aCommand.args->size() == 2)
    {
        std::string fileToDownload(aCommand.args->at(0));
        node_t* nodeFile = findNodeAtDirectory(tree->getActualDirectoryNode(), fileToDownload);
        if(nodeFile != NULL)
        {
            if(!nodeFile->directoryFlag)
            {
                char* fileData = (char*)malloc(nodeFile->size*sizeof(char));

                raid->readFile(fileData, nodeFile->size, nodeFile->vectorOfBlocksId);

                FILE* newFile = fopen(fileToDownload.c_str(), "wb");
                fwrite(fileData, sizeof(char), nodeFile->size, newFile);
                fclose(newFile);
            }
            else
            {
                std::cout << fileToDownload.c_str() << " no es un archivo" << std::endl;
            }
        }
        else
        {
            std::cout << "Archivo no encontrado..." << std::endl;
        }
    }
}
/*!
 * \brief Terminal::mv Método para cambiar de nombre un fichero/directorio en nuestro árbol
 * \param aCommand es el comando entero para cambiar el nombre
 * Tenemos que ver si el comando acaba en "/" o ".", porque eso cambiará el comportamiento
 * Una vez encontremos el nodo, vamos a updatearlo seǵun sea dir o fichero.
 *
 */
void Terminal::mv(command_t aCommand)
{
    if(aCommand.args->size() == 3)
    {
        std::string nodeNameOrig(aCommand.args->at(0));
        std::string nodeNameDest(aCommand.args->at(1));
        node_t* targetNode = NULL;

        if(std::string::npos == nodeNameDest.find("/"))
        {
            targetNode = findNodeAtDirectory(tree->getActualDirectoryNode(), nodeNameOrig);

            if(targetNode != NULL)
            {
                if(targetNode->directoryFlag && NULL == findNodeAtDirectory(tree->getActualDirectoryNode(), nodeNameDest) && std::string::npos == nodeNameDest.find("."))
                {
                    tree->updateNode(targetNode->id, nodeNameDest, targetNode->size);
                }
                else if(!targetNode->directoryFlag && NULL == findNodeAtDirectory(tree->getActualDirectoryNode(), nodeNameDest))
                {
                    tree->updateNode(targetNode->id, nodeNameDest, targetNode->size);
                }
                else
                {
                    std::cout << "Error: You can not use same name for 2 files or directories or name a directory with \".\" " << std::endl;
                }
            }
            else
            {
                std::cout << "This file or directory doesn't exist" << std::endl;
            }
        }
        else
        {
            std::cout << "Error: You can not use special characters" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }
}
/*!
 * \brief Terminal::cp Método para copiar un nodo en una posición
 * \param aCommand es el comando entero para copiar el nodo y el lugar
 * Primero tenemos que saber si acaba en "/" para saber si ese va a ser el nuevo nombre
 * Si hay una "/" significa que el nombre final será el del nodo original
 * Luego iremos actualizando el destino final del nodo a copiar,
 * Con esa información, cogeremos el nodo origen e iremos buscando de manera recursiva ese nodo
 * Lo iremos copiando todo también de forma recursiva
 * Si el nodo origen NO es un directorio, lo escribiremos en disco, ya que "copiamos" uno (creamos)
 */
void Terminal::cp(command_t aCommand)
{
    if(aCommand.args->size() == 3)
    {
        std::string nodeNameOrigin(aCommand.args->at(0));
        std::string destNodeName;
        char* destName;
        node_t* originNode = NULL;
        node_t* destNode = NULL;


        std::string otraForma(aCommand.args->at(1));
        char* destPathCopy=(char*)otraForma.c_str();

        //strcpy(destPathCopy, aCommand.args->at(1));

        if(!strncmp(&(destPathCopy[strlen(destPathCopy)-1]), "/", 1))
        {
            destNodeName = nodeNameOrigin;
        }
        else
        {
            char* token;

            token = strtok(destPathCopy, "/\n");
            while(token != NULL)
            {
                destName = token;
                token = strtok(NULL, "/\n");
            }
            destNodeName = std::string(destName);
        }

        originNode = findNodeAtDirectory(tree->getActualDirectoryNode(), nodeNameOrigin);

        destNode = findByPath(aCommand.args->at(1));

        if(originNode != NULL && destNode != NULL)
        {
            if(originNode->directoryFlag)
            {
                if(findNodeAtDirectory(destNode, destNodeName) == NULL)
                {
                    if(!findSubdirectoryDependency(originNode, destNode))
                    {
                        copyDirectoryRecursive(originNode, destNode, destNodeName);
                    }
                    else
                    {
                        std::cout << "Error: Can not copy a directory in one of its subfolders" << std::endl;
                    }

                }
                else
                {
                    std::cout << "Error: You can not use same name for 2 directories" << std::endl;
                }
            }
            else
            {

                if(findNodeAtDirectory(destNode, destNodeName) == NULL)
                {

                    //FILE* tempFile = fopen(aCommand.args->at(0), "rb");
                    char* dataFile = (char*)malloc(sizeof(char)*originNode->size);

                    //fread(dataFile, sizeof(char), originNode->size, tempFile);
                    raid->readFile(dataFile,originNode->size,originNode->vectorOfBlocksId);
                    std::vector<int> fileBlocksId = raid->writeFile(dataFile, originNode->size);
                    tree->addNode(destNode, destNodeName, false, originNode->size, fileBlocksId);


                    free(dataFile);
                }

                else
                {
                    std::cout << "Error: You can not use same name for 2 files" << std::endl;
                }

            }
        }
        else
        {
           std::cout << "This file or directory doesn't exist" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }

}
/*!
 * \brief Terminal::lls hacer el ls en nuestro árbol local
 */
void Terminal::lls()
{
    system("ls");
}
/*!
 * \brief Terminal::lcd Método para CD en nuestro árbol local
 * \param aCommand es el lugar donde nos vamos a mover
 * Realizaremos el CD en nuestro árbol.
 */
void Terminal::lcd(command_t aCommand)
{
    if(aCommand.args->size() == 2)
    {
        std::string args(aCommand.args->at(0));
        chdir(args.c_str());
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }
}
/*!
 * \brief Terminal::lpwd sacar la direccion pwd en el árbol local
 */
void Terminal::lpwd()
{
    system("pwd");
}
/*!
 * \brief Terminal::touch método para crear un nodo
 * \param aCommand el comando para la creación
 * Si no existe el nodo donde queremos crearlo, lo creamos.
 */
void Terminal::touch(command_t aCommand)
{
    if(aCommand.args->size() == 2)
    {
        std::string nameOfDir = aCommand.args->at(0);
        if(std::string::npos == nameOfDir.find("/"))
        {
            if(findDirectoryAtDirectory(tree->getActualDirectoryNode(), nameOfDir) == NULL)
            {
                tree->addNode(tree->getActualDirectoryNode(), nameOfDir, false, 0);
            }
            else
            {
                std::cout << "Error: You can not use the same name as other directory" << std::endl;
            }


        }
        else
        {
            std::cout << "Error: You can not use special characters" << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid number of arguments" << std::endl;
    }
}

/*!
 * \brief Terminal::run Método que carga el árbol y lee/ejecuta comandos
 * Este método es el, por así decirlo, el que inicia todo.
 * Carga el árbol, y es el que va a ir leyendo y ejecutando los comandos.
 */
void Terminal::run(){
    command_t command;
    command.args = new std::vector<char*>();

    tree->loadTree();

    while(!exit){
        command.clean();
        std::cout << "$: ";
        fflush(stdout);
        readCommand(&command);
        runCommand(command);
    }
    tree->saveTree();
}
/*!
 * \brief Terminal::readCommand Método para leer un comando
 * \param aCommand El comando a leer
 * Utilizaremos el tokenizer para saber diferenciar las partes del comando
 * Iremos añadiendo los parametros del comando mientras podamos seguir leyendo.
 */
void Terminal::readCommand(command_t* aCommand){
    char * line= new char [1024];
    char spacer[3]=" \n";
    char* token=NULL;

    fgets(line, 1023, stdin);
    token=strtok(line, spacer);
    aCommand->type=getTypeOfCommand(token);

    while(token!=NULL){
        token=strtok(NULL, spacer);
        aCommand->args->push_back(token);
    }
}
/*!
 * \brief Terminal::getTypeOfCommand Método que según lo que le mandemos, nos dice el comando que es
 * \param aCommandArray es el comando que le pasamos
 * \return command_e será el tipo de comando que nos devuelve.
 * en este caso, según lo que leamos, nos devolverá el tipo de comando que es.
 * Será el método utilizado cuando leamos un comando
 */
command_e Terminal::getTypeOfCommand(char* aCommandArray)
{
    if(aCommandArray != NULL)
    {
        if(!strncmp("cd", aCommandArray, 3))
            return command_e::CD;
        else if(!strncmp("ls\0", aCommandArray, 3))
            return command_e::LS;
        else if(!strncmp("pwd\0", aCommandArray, 4))
            return command_e::PWD;
        else if(!strncmp("mv\0", aCommandArray, 3))
            return command_e::MV;
        else if(!strncmp("cp\0", aCommandArray, 3))
            return command_e::CP;
        else if(!strncmp("mkdir\0", aCommandArray, 6))
            return command_e::MKDIR;
        else if(!strncmp("rmdir\0", aCommandArray, 6))
            return command_e::RMDIR;
        else if(!strncmp("rm\0", aCommandArray, 3))
            return command_e::RM;
        else if(!(strncmp("lls\0", aCommandArray, 4)))
            return command_e::LLS;
        else if(!strncmp("lcd\0", aCommandArray, 4))
            return command_e::LCD;
        else if(!strncmp("lpwd\0", aCommandArray, 5))
            return command_e::LPWD;
        else if(!strncmp("upload\0", aCommandArray, 7))
            return command_e::UPLOAD;
        else if(!strncmp("exit\0", aCommandArray, 5))
            return command_e::END;
        else if(!strncmp("download\0", aCommandArray, 9))
            return command_e::DOWNLOAD;
        else if(!strncmp("touch\0", aCommandArray, 9))
            return command_e::TOUCH;
    }
    return command_e::NO_COMMAND;
}
/*!
 * \brief Terminal::runCommand Método para ejecutar un comando
 * \param aCommand es el comando a ejecutar
 * Según el comando que le pasemos, ejecutará en consola uno u otro.
 * Hemos puesto control de errores en el mismo
 */
void Terminal::runCommand(command_t aCommand)
{
    switch(aCommand.type)
    {
        case command_e::CD:
            cd(aCommand);
            break;
        case command_e::LS:
            if(aCommand.args->size() == 1)
            {
                ls();
            }
            else
            {
                std::cout << "ls no admite parametros" << std::endl;
            }
            break;
        case command_e::PWD:
            if(aCommand.args->size() == 1)
            {
                pwd();
            }
            else
            {
                std::cout << "pwd no admite parametros" << std::endl;
            }
            break;
        case command_e::MV:
            mv(aCommand);
            break;
        case command_e::CP:
            cp(aCommand);
            break;
        case command_e::MKDIR:
            mkdir(aCommand);
            break;
        case command_e::RMDIR:
            rmdir(aCommand);
            break;
        case command_e::RM:
            rm(aCommand);
            break;
        case command_e::LLS:
            if(aCommand.args->size() == 1)
            {
                lls();
            }
            else
            {
                std::cout << "lls no admite parametros" << std::endl;
            }
            break;
        case command_e::LCD:
            lcd(aCommand);
            break;
        case command_e::LPWD:
            if(aCommand.args->size() == 1)
            {
                lpwd();
            }
            else
            {
                std::cout << "lpwd no admite parametros" << std::endl;
            }
            break;
        case command_e::UPLOAD:
            upload(aCommand);
            break;
        case command_e::END:
            exit = true;
            break;
        case command_e::DOWNLOAD:
            download(aCommand);
            break;
        case command_e::TOUCH:
            touch(aCommand);
            break;
        case command_e::NO_COMMAND:
            std::cout << "Command not found" << std::endl;
            break;
        default:
            std::cout << "Command not found" << std::endl;
            break;
    }
}
