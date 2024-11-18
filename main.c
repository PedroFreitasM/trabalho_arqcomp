#include <stdio.h>
#include <stdlib.h>
#include "fila.h"


#define QUANTUM_ALTA 10
#define QUANTUM_BAIXA 15

int main(int argc, char *argv[]) {
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
    processo **lista_processos = NULL;
    int process_count = 0;
    char *linha;

    while ((linha = get_linha(arq)) != NULL) {
        processo *p = criar_processo(linha);
        if (p != NULL) {
            lista_processos = (processo **)realloc(lista_processos, sizeof(processo *) * (process_count + 1));
            lista_processos[process_count++] = p;
        }
        free(linha);
    }
    fclose(arq);

    // Inicializar filas
    queue fila_alta_prioridade;
    queueInit(&fila_alta_prioridade);

    queue fila_baixa_prioridade;
    queueInit(&fila_baixa_prioridade);

    queue fila_io;
    queueInit(&fila_io);

    int tempo = 0;
    int processos_concluidos = 0;

    printf("\n[ INICIANDO SIMULAÇÃO ]\n\n\n");

    while (processos_concluidos < process_count) {
        printf("\n[ Ciclo %d ]\n", tempo);

        // Verificar processos que estão chegando e inserir na fila de alta prioridade
        for (int i = 0; i < process_count; i++) {
            processo *p = lista_processos[i];
            if (p->tempo_entrada == tempo && p->status == 0) {
                queueInserir(&fila_alta_prioridade, p);
                p->status = 1; // Pronto
                printf("[ Tempo %d: Processo %d chegou e foi inserido na fila de alta prioridade ]\n", tempo, p->pid);
            }
        }

        // Atualizar processos em I/O
        queue_nodo *nodo_io = fila_io.inicio;
        queue_nodo *prev_io = NULL;
        while (nodo_io != NULL) {
            processo *p = nodo_io->p;
            p->ios[p->prox_io - 1].tempo--;
            if (p->ios[p->prox_io - 1].tempo <= 0) {
                p->status = 1; // Pronto
                queueInserir(&fila_alta_prioridade, p);
                printf("[ Tempo %d: Processo %d concluiu operação de I/O e foi movido para a fila de alta prioridade ]\n", tempo, p->pid);
                // Remover da fila de I/O
                if (prev_io == NULL) {
                    fila_io.inicio = nodo_io->prox;
                } else {
                    prev_io->prox = nodo_io->prox;
                }
                if (nodo_io == fila_io.fim) {
                    fila_io.fim = prev_io;
                }
                queue_nodo *temp_io = nodo_io;
                nodo_io = nodo_io->prox;
                free(temp_io);
                continue;
            }
            prev_io = nodo_io;
            nodo_io = nodo_io->prox;
        }

        processo *processo_executando = NULL;
        int quantum_atual = 0;

        if (!queueVazio(fila_alta_prioridade)) {
            // Executar processo da fila de alta prioridade
            processo_executando = queueRemover(&fila_alta_prioridade);
            quantum_atual = QUANTUM_ALTA;
            printf("[ CPU executando: Processo %d (Alta Prioridade) ]\n", processo_executando->pid);
        } else if (!queueVazio(fila_baixa_prioridade)) {
            // Executar processo da fila de baixa prioridade
            processo_executando = queueRemover(&fila_baixa_prioridade);
            quantum_atual = QUANTUM_BAIXA;
            printf("[ CPU executando: Processo %d (Baixa Prioridade) ]\n", processo_executando->pid);
        } else {
            // CPU ociosa
            printf("[ CPU ociosa ]\n");
            tempo++;
            continue;
        }

        // Executar o processo pelo quantum ou tempo restante, o que for menor
        int tempo_execucao = (processo_executando->tempo_cpu - processo_executando->tempo_usado < quantum_atual)
                                ? (processo_executando->tempo_cpu - processo_executando->tempo_usado)
                                : quantum_atual;

        // Verificar se o processo precisa realizar I/O durante a execução
        int tempo_ate_prox_io = -1;
        if (processo_executando->io_qtd > 0 && processo_executando->prox_io < processo_executando->io_qtd) {
            tempo_ate_prox_io = processo_executando->ios[processo_executando->prox_io].tempo_inicio - processo_executando->tempo_usado;
            if (tempo_ate_prox_io <= tempo_execucao && tempo_ate_prox_io >= 0) {
                tempo_execucao = tempo_ate_prox_io;
            }
        }

        processo_executando->tempo_usado += tempo_execucao;
        tempo += tempo_execucao;

        printf("[ Processo %d executou por %d unidades de tempo (Tempo total usado: %d) ]\n",
               processo_executando->pid, tempo_execucao, processo_executando->tempo_usado);

        // Verificar se o processo terminou
        if (processo_executando->tempo_usado >= processo_executando->tempo_cpu) {
            processo_executando->status = 2; // Concluído
            processos_concluidos++;
            printf("[ Tempo %d: Processo %d concluído ]\n", tempo, processo_executando->pid);
        } else if (processo_executando->io_qtd > 0 && processo_executando->prox_io < processo_executando->io_qtd &&
                   processo_executando->tempo_usado >= processo_executando->ios[processo_executando->prox_io].tempo_inicio) {
            // Processo precisa realizar operação de I/O
            processo_executando->status = 3; // I/O
            queueInserir(&fila_io, processo_executando);
            printf("[ Tempo %d: Processo %d iniciou operação de I/O (Tipo %d) ]\n",
                   tempo, processo_executando->pid, processo_executando->ios[processo_executando->prox_io].tipo);
            processo_executando->prox_io++;
        } else {
            // Reenfileirar o processo na fila apropriada
            if (quantum_atual == QUANTUM_ALTA) {
                // Move para fila de baixa prioridade
                queueInserir(&fila_baixa_prioridade, processo_executando);
                printf("[ Processo %d foi movido para a fila de baixa prioridade\n ]", processo_executando->pid);
            } else {
                // Mantém na fila de baixa prioridade
                queueInserir(&fila_baixa_prioridade, processo_executando);
                printf("[ Processo %d continua na fila de baixa prioridade\n ]", processo_executando->pid);
            }
        }

        // Mostrar estado das filas
        printf(" Fila de Alta Prioridade: ");
        queuePrint(fila_alta_prioridade);
        printf("Fila de Baixa Prioridade: ");
        queuePrint(fila_baixa_prioridade);
        printf("Fila de I/O: ");
        queuePrint(fila_io);
    }

    // Limpar memória
    queueReset(&fila_alta_prioridade);
    queueReset(&fila_baixa_prioridade);
    queueReset(&fila_io);

    for (int i = 0; i < process_count; i++) {
        if (lista_processos[i]->ios != NULL) {
            free(lista_processos[i]->ios);
        }
        free(lista_processos[i]);
    }
    free(lista_processos);

    printf("\n\n[ SIMULAÇÃO CONCLUÍDA ]\n\n");

    return 0;
}