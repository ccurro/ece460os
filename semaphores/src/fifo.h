#ifndef FIFO_H
#define FIFO_H

#include "common.h"
#include "sem.h"

#define MYFIFO_BUFSIZ	4096

typedef struct fifo {
	unsigned long * fbuf;
	struct sem * s;
	struct sem * fillCount;
	struct sem * emptyCount;
	struct sem * mutex;
	int * in;
	int * out;
	int * counts;
} fifo;

void fifo_init(struct fifo * f);
int fifo_wr(struct fifo * f, unsigned long * buf, int bufSize);
int fifo_rd(struct fifo * f, unsigned long * buf, int bufSize);

#endif