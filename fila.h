#include "structs.h"
#include "stdbool.h"

typedef struct Link
{
    Processo processo;
    Link *link;

} No;

typedef struct
{
    No *inicio, *fim;

} Fila;

void filaInit (Fila *fila);

void filaTamanho (Fila fila);

bool filaVazia (Fila fila);

void filaPrint (Fila fila);

bool filaInserir (Fila *fila);

Processo filaRemover (Fila *fila);

void filaLimpar (Fila *fila);