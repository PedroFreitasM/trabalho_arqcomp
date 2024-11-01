#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSOS 10
#define QUANTUM 2




typedef struct{
    int pid;
    int tempo_servico;
    int instante_ativacao;
    int prioridade_inicial;
    int prioridade_dinamica;
}Processo;

int main(){
    processo* prioridade_baixa;
    prioridade_baixa = (processo *)malloc(processos_fila * sizeof(processo));

}