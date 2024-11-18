#include "structs.h"
#include "stdbool.h"

void queueInit(queue *q);

int queueTam(queue q);

bool queueVazio(queue q);

void queuePrint(queue q);

bool queueInserir(queue *q, processo *np);

processo *queueRemover(queue *q);

void queueReset(queue *q);
