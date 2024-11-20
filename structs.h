#include <stdio.h>
#include <stdlib.h>
#include "input_txt.h"

#define DISCO 0
#define FITA_MAG 1
#define IMPRESSORA 2

typedef struct io{
    int tipo;
    int tempo;
    int tempo_inicio;
    int tempo_restante;
} io;

typedef struct Processo {
    int pid;
    //Status e prioridade podem iniciar como 0 sempre (?)
    //Esses dois podem ir mudando no round robin 
    int status;
    int prioridade;

    int tempo_entrada;
    int tempo_cpu;

    int tempo_usado;

    io *ios;
    int io_qtd;
    int prox_io;

    int cota;

} processo;

typedef struct queue_nodo{
    processo *p;
    struct queue_nodo *prox;
}queue_nodo;

typedef struct queue{
    queue_nodo *inicio;
    queue_nodo *fim;
} queue;

typedef struct lista_link
{
	processo *p;
	struct lista_link *prox;
}lista_nodo;

typedef struct
{
	lista_nodo *topo;
}lista;

processo *criar_processo(char* linha)
{
    puts("");
    printf("Novo processo: %s\n\n", linha);
    int i = 0;
    int pid = get_int(linha, &i);
    printf("PID: %d\n", pid);
    int tempo_chegada = get_int(linha, &i);
    printf("Tempo de entrada: %d\n", tempo_chegada);
    int t_cpu = get_int(linha, &i);
    printf("Tempo de cpu: %d\n\n", t_cpu);
    int quant_io = get_int(linha, &i);
    printf("Quantia de I/Os: %d\n", quant_io);

    processo* p = (processo*)malloc(sizeof(processo));
    if(!p){
        printf("Falha na alocação do processo!\n");
        return NULL;
    }

    p->pid = pid;
    p->status = 0;
    p->prioridade = 0;
    p->tempo_entrada = tempo_chegada;
    p->tempo_cpu = t_cpu;

    p->tempo_usado = 0;
    //Já que o processo é novo...

    p->io_qtd = quant_io;
    puts("");

    if(quant_io > 0){
        p->ios = (io*)malloc(quant_io * sizeof(io));

        for(int j=0; j < quant_io; j++){
            int tipo_io = get_int(linha, &i);
            int tempo_io = get_int(linha, &i);
            int tempo_inicio = get_int(linha, &i);
            printf("Tipo de IO: %d \nTempo do IO: %d \nMomento de inicio: %d \n\n", tipo_io, tempo_io, tempo_inicio);
            p->ios[j].tipo = tipo_io;
            p->ios[j].tempo = tempo_io;
            p->ios[j].tempo_inicio = tempo_inicio;
            p->ios[j].tempo_restante=tempo_io;
        }
    }
    p->prox_io = 0;
    p->cota = 0;

    return p;
}
