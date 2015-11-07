#include "fifo.h"

void fifo_init(struct fifo * f) {
	f->counts = mmap(NULL, sizeof(int)*2, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0 );
	f->fbuf = mmap(NULL, sizeof(unsigned long)*MYFIFO_BUFSIZ, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0 );
	f->s = mmap(NULL, sizeof(struct sem)*3, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0 );	

	mapChk(f->counts); 
	mapChk(f->fbuf);
	mapChk(f->s);

	f->in = &f->counts[0];
	f->out = &f->counts[1];

	f->mutex = &f->s[2];
	f->fillCount = &f->s[1];
	f->emptyCount = &f->s[0];

	sem_init(f->mutex,1);
	sem_init(f->fillCount,0);
	sem_init(f->emptyCount,MYFIFO_BUFSIZ);
}

int fifo_wr(struct fifo * f, unsigned long * buf, int bufSize) {
	for (int i = 0; i < bufSize; i++) {
		while (1) {
			sem_wait(f->emptyCount);

			if(sem_try(f->mutex)) {
				f->fbuf[*f->in] = buf[i];
				*f->in = (*f->in + 1) % MYFIFO_BUFSIZ;
				sem_inc(f->mutex);
				sem_inc(f->fillCount);
				break;
			} else {
				sem_inc(f->emptyCount);
			}
		}
	}
}	

int fifo_rd(struct fifo * f, unsigned long * buf, int bufSize) {
	for (int i = 0; i < bufSize; i++) {
		while(1) {
			sem_wait(f->fillCount);

			if(sem_try(f->mutex)) {
				buf[i] = f->fbuf[*f->out];
				*f->out = (*f->out + 1) % MYFIFO_BUFSIZ;
				sem_inc(f->mutex);
				sem_inc(f->emptyCount);
				break;
			} else {
				sem_inc(f->fillCount);
			}
		}
	}
	return bufSize;
}
