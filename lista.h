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

processo **queueObter(queue *q) 
{
	return &q->inicio->p;
}



//coisas de lista ligada
void listaInit(lista *l)
{
	l->topo=NULL;
}

int listaTam(lista l)
{
	int t=0;
	while(l.topo!=NULL)
	{
		l.topo = l.topo->prox;
		t++;
	}	
	return t;
}

bool listaVazia(lista l)
{
	if(l.topo==NULL)
		return true;
		
	return false;
}

void listaPrint(lista l)
{
	putchar('[');
	while(l.topo!=NULL)
	{
		printf("pid:%d,", l.topo->p->pid);
		printf("status:%d,", l.topo->p->status);
		printf("prioridade:%d;", l.topo->p->prioridade);
		l.topo=l.topo->prox;
	}
	putchar(']');
	puts("");
}

bool listaInserir(lista *l, processo *np, int pos)
{
	if(pos<0||pos>listaTam(*l)||np==NULL)
		return false;
		
	lista_nodo *novo = (lista_nodo*)malloc(sizeof(lista_nodo));
	if(novo==NULL)
		return false;
		
	novo->p=np;
	novo->prox=NULL;
			
	if(listaVazia(*l))
	{
		l->topo=novo;
		return true;
	}
	
	lista_nodo *tmp=l->topo;
	
	for(int i=1;i<pos;i++)
		tmp=tmp->prox;
		
	novo->prox=tmp->prox;
	tmp->prox=novo;
	tmp=NULL;
	
	return true;
}

processo *listaRemover(lista *l, int pos)
{
	if(pos<0||pos>=listaTam(*l)||listaVazia(*l))
		return NULL;
		
	processo *getp=NULL;
	
	lista_nodo *tmp=l->topo;
	
	if(pos==0)
	{
		getp=tmp->p;
		l->topo=l->topo->prox;
		
		tmp->p=NULL;
		tmp->prox=NULL;
		free(tmp);
		
		return getp;
	}
	
	for(int i=1;i<pos;i++)
		tmp=tmp->prox;
		
	getp=tmp->prox->p;
	lista_nodo *tmp2=tmp->prox;
	tmp->prox=tmp->prox->prox;

	tmp2->p=NULL;
	tmp2->prox=NULL;
	free(tmp2);
	
	return getp;
}

void listaReset (lista *l)
{
	while (listaRemover(l,0)!=NULL);
}

processo *listaVer(lista l, int pos)
{
	if(pos<0||pos>=listaTam(l))
		return NULL;
		
	for(int i=0;i<pos;i++)
		l.topo=l.topo->prox;
		
	return l.topo->p;
}

processo **listaObter(lista *l, int pos) 
{
	if(pos<0||pos>=listaTam(*l))
		return NULL;
		
	lista_nodo *tmp = l->topo;
	for(int i=0;i<pos;i++)
		tmp=tmp->prox;
		
	return &tmp->p;
}
