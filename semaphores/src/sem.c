#include "common.h"
#include "sem.h"

void handler(int signo) {}

void sem_init(struct sem * s, int count) {
	s->count = count;
	s->lock = 0;

	memset(s->waiting,0,NPROCS);

	sigfillset(&s->sigmask);
	sigdelset(&s->sigmask,SIGINT);
	sigdelset(&s->sigmask,SIGUSR1);
	condChk(signal(SIGUSR1,handler) == SIG_ERR, "Failed to set signal handler\n");
}

void sem_inc(struct sem * s) {
	spinLock(&s->lock);
	s->count++;
	for (int i = 0; i < NPROCS; i++) {
		if (s->waiting[i]) {
			kill(s->pids[i],SIGUSR1);
			break;
		}
	}
	unLock(&s->lock);
}

int sem_try(struct sem * s) {
	spinLock(&s->lock);
	if (s->count > 0) {
		s->count--;
		unLock(&s->lock);
		return 1;
	} else {
		unLock(&s->lock);
		return 0;
	}
}

void sem_wait(struct sem * s) {
	s->waiting[my_procnum] = 1;
	s->pids[my_procnum] = getpid();

	while (1) {
		spinLock(&s->lock);
		if (s->count > 0 ) {
			s->waiting[my_procnum] = 0;
			s->count--;			
			unLock(&s->lock);
			return;
		} else {
			unLock(&s->lock);
			sigsuspend(&s->sigmask);
		}
	}
}
