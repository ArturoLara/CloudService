#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>
#include <cstring>

Terminal::Terminal()
{
    tree = new DataTree();
}


void Terminal::run(){
    bool exit=false;
    command_t command;
    command.args= new vector<char*>();
    while(!exit){
        lee_comando(&command);
        ejecuta_comando(&comando);
    }
}
void Terminal::lee_comando(command_t* command){
    char* line=new char [1024];
    char spacer[2]=" ";
    char* token=NULL;
    
    //leer terminal
    fgets(line, 1023, stdin);
    //subdividir en argumentos
        //string tokenizer
        //ej: cd .., tokenizer devolvera cd en token
    //gestion de errores:
        //User no introdujo comando
    token=strtok(line, spacer);
    command->type=get_tipo_comando(token);
    while(token!=NULL){
        token=strtok(NULL, spacer);
        comando->argumentos->push_back(token);
    }
}

command_e Terminal::get_tipo_comando(char* commandArray)
{
    if(commandArray != NULL)
    {
        if(strncmp("cd", commandArray, 2))
            return command_e::cd;
        else if(strncmp("ls", commandArray, 2))
            return command_e::ls;
        else if(strncmp("pwd", commandArray, 3))
            return command_e::pwd;
        else if(strncmp("mv", commandArray, 2))
            return command_e::mv;
        else if(strncmp("cp", commandArray, 2))
            return command_e::cp;
        else if(strncmp("mkdir", commandArray, 5))
            return command_e::mkdir;
        else if(strncmp("rmdir", commandArray, 5))
            return command_e::rmdir;
        else if(strncmp("rm", commandArray, 2))
            return command_e::rm;
        else if(strncmp("lls", commandArray, 3))
            return command_e::lls;
        else if(strncmp("lcd", commandArray, 3))
            return command_e::lcd;
        else if(strncmp("lpwd", commandArray, 4))
            return command_e::lpwd;
        else if(strncmp("upload", commandArray, 6))
            return command_e::upload;
        else if(strncmp("exit", commandArray, 4))
            return command_e::exit;

    }
    return command_e::noCommand;
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

using namespace std;

int main(){
    Terminal* term=new Terminal();
    term->run();
    return 0;
}
