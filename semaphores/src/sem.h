#ifndef SEM_H
#define SEM_H

#include "common.h"

int my_procnum;

typedef struct sem {
	int count;
	int lock;
	int waiting[NPROCS];
	int pids[NPROCS];
	sigset_t sigmask;
} sem;

void sem_init(struct sem * s, int count);
void sem_wait(struct sem * s);
void sem_inc(struct sem * s);
int sem_try(struct sem * s);

#endif