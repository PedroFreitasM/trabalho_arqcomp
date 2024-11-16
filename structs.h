#include <stdio.h>
#include <stdlib.h>

#define DISCO 0
#define FITA_MAG 1
#define IMPRESSORA 2

typedef struct io{
    int tipo;
    int tempo;
}

typedef struct Processo {
    int pid;
    int status;
    int prioridade;

    int tempo_entrada;
    int tempo_cpu;

    int tempo_usado;

    io *ios;
    int io_qtd;
    int prox_io;

} processo;

typedef struct queue_nodo{
    processo *p;
    struct queue_nodo *prox;
}queue_nodo;

typedef struct queue{
    queue_nodo *inicio;
    queue_nodo *fim;
} queue;