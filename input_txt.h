#include <stdio.h>
#include <stdlib.h>

//As funções interpreta o ; como fim de int

#define BUFFER 256
//pega um int da linha 
int get_int(char* linha, int* pos){
    int num = 0;

    while(linha[*pos] != ';' && linha[*pos] != '\0'){
        num = num * 10 + (linha[*pos] - '0'); // gambiarra pra transformar string em int
        (*pos)++;
    }
    if(linha[*pos] == ';'){
    (*pos)++;
    }
    return num;
}


// Pega uma linha inteira do arquivo, começando onde o ponteiro está e indo até o \n ou EOF
char *get_linha (FILE* arq){
    int pos = 0;
    char *linha = (char *)malloc(BUFFER);
    if(!linha){
        printf("Falha na alocação da linha\n");
        return NULL;
    }
    int ch; // é melhor usar int, pode dar problema de interpretação
    //se precisar a gente muda dps


    while((ch = fgetc(arq)) != EOF && ch != '\n'){
        linha[pos] = ch;
        pos++;
    }
    // termina a string com \0
    linha[pos] = '\0';

    if (pos == 0 && ch == EOF){
        free(linha);
        return NULL;
    }

    return linha;
}
