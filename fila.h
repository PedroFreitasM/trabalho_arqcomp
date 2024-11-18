#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "structs.h"

void queueInit(queue *q)
{
	q->inicio=NULL;
	q->fim=NULL;
}

int queueTam(queue q)
{
	int tam=0;
	
	while(q.inicio!=NULL)
	{
		q.inicio=q.inicio->prox;
		tam++;
	}
	
	return tam;
}

bool queueVazio(queue q)
{
	if(q.inicio==NULL)
		return true;
		
	return false;
}

void queuePrint(queue q)
{
	putchar('[');
	while(q.inicio!=NULL)
	{
		printf("pid:%d,", q.inicio->p->pid);
		printf("status:%d,", q.inicio->p->status);
		printf("prioridade:%d;", q.inicio->p->prioridade);
		q.inicio=q.inicio->prox;
	}
	putchar(']');
	puts("");
}

bool queueInserir(queue *q, processo *np)
{
	queue_nodo *novo = (queue_nodo*) malloc(sizeof(queue_nodo));
	if(novo==NULL)
		return false;
		
	novo->p=np;
	novo->prox=NULL;

	if(queueVazio(*q))
	{
		q->inicio=novo;
		q->fim=novo;
		
		return true;
	}
	
	q->fim->prox=novo;
	q->fim=novo;
	
	return true;
}

processo *queueRemover(queue *q)
{
	if(queueVazio(*q))
		return NULL;
	
	processo *pret=q->inicio->p;
	
	if (queueTam(*q)==1)
	{
		free(q->inicio);
		q->inicio=NULL;
		q->fim=NULL;
		return pret;
	}
	
	queue_nodo *tmp=q->inicio->prox;
	free(q->inicio);
	q->inicio=tmp;
	return pret;
}

void queueReset(queue *q)
{
	while (queueRemover(q)!=NULL);
}

