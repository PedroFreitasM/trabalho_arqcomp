#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

#define QUANTUM 4

processo **processo_obter_preferido(queue fila_alta_prioridade, queue fila_baixa_prioridade);



int main(int argc, char *argv[])
{   
    if (argc < 2) {
        printf("Arquivo utilizado: %s arquivo_de_entrada\n", argv[0]);
        return 1;
    }

    FILE *arq = fopen(argv[1], "r");
    if (arq == NULL) {
        printf("Falha ao abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    // Ler processos do arquivo de entrada
    lista lista_processos; //processos nao inicializados aguardam numa lista ligada
    listaInit(&lista_processos);
    int process_count = 0;
    char *linha;

    while ((linha = get_linha(arq)) != NULL) {
        processo *p = criar_processo(linha);
        puts("");
        if (p != NULL) {
			listaInserir(&lista_processos, p, process_count);
			process_count++;
        }
        free(linha);
    }
    
    fclose(arq);

    // Inicializar filas
    queue fila_alta_prioridade, fila_baixa_prioridade;
    lista lista_io;
    queueInit(&fila_alta_prioridade);
    queueInit(&fila_baixa_prioridade);
    listaInit(&lista_io);

    int processos_concluidos = 0;
    int processos_nao_inicializados=process_count;

    printf("\n\n[INICIANDO SIMULACAO]\n\n\n");
    
    ///////////////////////////////////////////////////////////////////

    //loop principal de execucao
	for(int tempo=0; ; tempo++) 
	{
		printf("[CICLO %d]\n", tempo);

        //vamos observar e manipular o processo que está no topo
        processo **topo=NULL;

		//verificar processos que estão chegando e inserir na fila de alta prioridade
		for (int i = processos_nao_inicializados-1; i>=0; i--)
		{
			processo *p=listaVer(lista_processos,i);
			
			//caso chegue o tempo de entrada de um processo qualquer na lista de espera
			if(p->tempo_entrada==tempo)
			{
				//retirando o processo da lista de espera
				listaRemover(&lista_processos,i);
				processos_nao_inicializados--;
				//adicionando o processo a fila de alta prioridade
				queueInserir(&fila_alta_prioridade, p);
                p->status = 1; // Pronto

                printf("Processo %d iniciou e foi inserido na fila de alta prioridade.\n", p->pid);
			}
		}
        
        //avanco na execucao dos I/O's. vamos descontar o tempo de cada dispositivo. pode levar a reinsercao do processo na fila
        int qtd_io=listaTam(lista_io);
        processo *retorno_io=NULL;

        for(int i=qtd_io-1; i>=0; i--)
        {
            processo **p = listaObter(&lista_io, i);
            (*p)->ios[(*p)->prox_io-1].tempo_restante--;

            if((*p)->ios[(*p)->prox_io-1].tempo_restante == 0)
            {
                printf("processo: %d, I/O: %d resolvido.\n",(*p)->pid, (*p)->prox_io-1);

                retorno_io = listaRemover(&lista_io, i);

                if(retorno_io->ios[retorno_io->prox_io-1].tipo == DISCO)
                {
                    queueInserir(&fila_baixa_prioridade, retorno_io);
                    printf("Processo %d voltou para a fila de baixa prioridade.\n", retorno_io->pid);
                }
                else
                {
                    queueInserir(&fila_alta_prioridade, retorno_io);
                    printf("Processo %d voltou para a fila de alta prioridade.\n", retorno_io->pid);
                }
            }
            else
                printf("Processo: %d, I/O: %d, tempo restante: %d.\n", (*p)->pid, (*p)->prox_io-1, (*p)->ios[(*p)->prox_io-1].tempo_restante);

        }

        //o topo será o primeiro da fila alta, ou o primeiro da fila baixa, caso a anterior estaja vazia
        //caso ambas as filas estejam vazias, o topo será nulo
        topo = processo_obter_preferido(fila_alta_prioridade, fila_baixa_prioridade);

        if(topo!=NULL)
            printf("Topo: %d, tempo em execucao: %d/%d, cota: %d.\n", (*topo)->pid, ((*topo)->tempo_usado)+1, (*topo)->tempo_cpu, (*topo)->cota);

        //se o topo nao eh nulo, possui I/O's sobrando, e esta no momento de um I/O executar, iremos faze-lo
        while(topo!=NULL && (*topo)->prox_io!=(*topo)->io_qtd && (*topo)->ios[(*topo)->prox_io].tempo_inicio==(*topo)->tempo_usado)
        {
            printf("Processo %d interrompeu devido ao I/O %d (tipo ", (*topo)->pid, (*topo)->prox_io);
            switch((*topo)->ios[(*topo)->prox_io].tipo)
            {
                case DISCO:
                    puts("disco).");
                    break;

                case FITA_MAG:
                    puts("fita magnetica).");
                    break;

                case IMPRESSORA:
                    puts("impressora).");
                    break;

                default:
                    break;
            }
            (*topo)->prox_io++;
            (*topo)->cota=0;
            processo *p=queueVazio(fila_alta_prioridade)?queueRemover(&fila_baixa_prioridade):queueRemover(&fila_alta_prioridade);

            listaInserir(&lista_io, p, listaTam(lista_io));

            topo = processo_obter_preferido(fila_alta_prioridade, fila_baixa_prioridade);
            if(topo!=NULL)
                printf("Novo topo: %d, tempo em execucao: %d/%d, cota: %d.\n", (*topo)->pid, (*topo)->tempo_usado, (*topo)->tempo_cpu, (*topo)->cota);

        }

        //avanço na execucao de um processo. pode levar a substituicao ou finalizacao.
        if(topo!=NULL)
        {
            puts("Avancamos o tempo do processo em execucao.");
            (*topo)->tempo_usado++;
            (*topo)->cota++;

            //caso 1: programa finaliza
            if((*topo)->tempo_cpu==(*topo)->tempo_usado)
            {
                printf("Processo %d finalizou!\n", (*topo)->pid);

                processos_concluidos++;
                processo *p=queueVazio(fila_alta_prioridade)?queueRemover(&fila_baixa_prioridade):queueRemover(&fila_alta_prioridade);
                free(p);

                if(processos_concluidos==process_count)
                {
                    puts("Todos os processos terminaram!");
                    break;
                }

                topo = processo_obter_preferido(fila_alta_prioridade, fila_baixa_prioridade);
                if(topo!=NULL)
                    printf("Novo topo: %d\n", (*topo)->pid);
            }

            //caso 2: programa nao finaliza mas estoura quantum
            if((*topo)->cota==QUANTUM)
            {
                processo *p=queueVazio(fila_alta_prioridade) ? queueRemover(&fila_baixa_prioridade) : queueRemover(&fila_alta_prioridade);
                printf("Processo %d estourou o quantum. Foi para a fila de baixa prioridade.\n", p->pid);
                p->cota=0;
                queueInserir(&fila_baixa_prioridade,p);

                //nao eh necessario obter novo processo caso haja apenas um em ambas as filas
                if(queueVazio(fila_alta_prioridade) && queueVazio(fila_baixa_prioridade))
                {
                    topo = processo_obter_preferido(fila_alta_prioridade, fila_baixa_prioridade);
                    if(topo!=NULL)
                        printf("Novo topo: %d.\n", (*topo)->pid);
                }
            }
        }
        puts("\n");
    }

    //////////////////////////////////////////////////////////////////

    // Limpar memória
    queueReset(&fila_alta_prioridade);
    queueReset(&fila_baixa_prioridade);
    listaReset(&lista_io);

    printf("\n\n[SIMULACAO CONCLUIDA]\n\n");

    getchar();

    return 0;
}

processo **processo_obter_preferido(queue fila_alta_prioridade, queue fila_baixa_prioridade)
{
    if(!queueVazio(fila_alta_prioridade))
    {
        puts("Fila alta sera executada.");
        return queueObter(&fila_alta_prioridade);
    }
    else if(!queueVazio(fila_baixa_prioridade))
    {
        puts("Fila alta esta vazia. Fila baixa sera executada.");
        return queueObter(&fila_baixa_prioridade);
    }
    else
    {
        puts("Ambas as filas estao vazias.");
        return NULL;
    }
}