#ifndef TERMINAL_H
#define TERMINAL_H
#include "arbol.h"
#include <vector>
 #define CMD_CD 1
#define CMD_MKDIR 2
#define CMD_EXIT 

typedef struct comando_t{
    int tipo;
    std::vector<char*> argumentos;
}comando_t;

class Terminal{
    const char* comandos[2]={
        "cd", 
        "mkdir", 
        "exit"
    };
   

    public:
    Arbol* arbol;
    Terminal();
    void run();
    void lee_comando(comando_t* comando);
    void ejecuta_comando(comando_t comando);
    int get_tipo_comando(char* comando);
};
arbol.h y .cpp están vacíos. 
cpp:

#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
Terminal::Terminal()
{
    arbol=new ARbol();
}


void Terminal::run(){
    bool exit=false;
    int comando=-1;
    comando_t comando;
    comando.argumentos= new vector<char*>();
    while(!exit){
        lee_comando(&comando);
        ejecuta_comando(&comando);
    }
}
void Terminal:: lee_comando(comando_t* comando){
    char* linea=new char [1024];
    char separador[2]=" ";
    char* token=NULL;
    
    //leer terminal
    fgets(linea, 1023, stdin);
    //subdividir en argumentos
        //string tokenizer
        //ej: cd .., tokenizer devolvera cd en token
    //gestion de errores:
        //User no introdujo comando
    token=strtok(linea, separador);
    comando->tipo=get_tipo_comando(token);
    while(token!=NULL){
        token=strtok(NULL, separador);
        comando->argumentos->push_back(token);
    }
}

int Terminal::get_tipo_comando(char* comando)
{
    if(comando != NULL)
    {
        if(strncmp("cd", comando, 2))
            return CDM_CD;
        if(strncmp("mkdir", comando, 5))
            return CDM_MKDIR;
        if(strncmp("exit", comando, 5))
            return CMD_EXIT;
    }
    retur -1
}
void Terminal::ejecuta_comando(comando_t* comando)
{
    switch(comando->tipo)
    {
        case CMD_CD:
            break;
        case CMD_MKDIR:
            break;
        case CMD_EXIT: 
            //salvar cozas
            break;
        default:
            std::cout << "Command not found" << std::endl;
            break;
    }
}

//MAIN//////////////////////////////////

#include <iostream>
#include "terminal.h"
using namespace std;

int main(){
    Terminal* term=new Terminal();
    term->run();
    return 0;
}