#ifndef SCHED_H
#define SCHED_H

#define TICK_RATE 0.001
#define SCHED_NPROC 256
/* Suggested task state values */
#define SCHED_READY 1
#define SCHED_RUNNING 2
#define SCHED_SLEEPING 3
#define SCHED_ZOMBIE 4

typedef struct sched_proc {
	void * stack_basePtr;
	void * stack_stackPtr;
	struct savectx * ctx;
	int state;
	pid_t pid;
	pid_t ppid;
	int taskAssigned;
} sched_proc;

typedef struct sched_waitq {
	struct sched_proc * procs[SCHED_NPROC];
	pid_t pids[SCHED_NPROC];
} sched_waitq;

void waitq_init();
void sched_init(void (*init_fn)());
pid_t sched_fork();
void sched_exit(int code);
int sched_wait(int *exit_code);
int sched_nice(int niceval);
pid_t sched_getpid();
pid_t sched_getppid();
pid_t sched_getfreepid();
int sched_gettick();
void sched_ps();
void sched_switch();
void sched_tick();
void adjstack(void *lim0,void *lim1,unsigned long adj);
#endif