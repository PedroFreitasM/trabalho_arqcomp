#include <stdio.h>
#include <stdlib.h>
#include "fila.h"
#include "structs.h"

#define quantum 4

void iniciar(char *nome) {
    
    printf("%s \n\n", nome);
}









int main(int argc, char *argv[]) {
    iniciar(argv[1]);
    round_robin();

}