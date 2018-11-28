/*
 * Authors: Mario Cavero y Arturo Lara
*/

#include "terminal.h"
#include <iostream>

int main(int argc, char **argv){
    MPI_Init(&argc, &argv);
    Terminal* term=new Terminal();
    term->run();
    return 0;
}
