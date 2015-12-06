#include "common.h"
#include "sched.h"

#define STACK_SIZE 64000

struct sched_proc * current;
int nTicks = 0;
sched_waitq waitq;
struct savectx fork_ctx;

void sched_switch() {
	fprintf(stderr,"In switching routine\n");
	for (int i = 0; i < SCHED_NPROC; i++) {
		if (waitq.procs[i]->state == SCHED_READY) {
			waitq.procs[i]->state = SCHED_RUNNING;
			current->state = SCHED_READY;
			fprintf(stderr,"Switched to pid %d\n", waitq.procs[i]->pid);
			restorectx(waitq.procs[i]->ctx,1);
		}
	}
}

void sched_tick(int signo) {
	sigset_t old_mask = block();
	nTicks++;

	struct savectx ctx;
	int jmped;
	errReport(jmped = savectx(&ctx),"Failed to save context: ");
	if (jmped == 0) {
		fprintf(stderr,"jmped\n");
		sched_switch();
	}

	fprintf(stderr,"Returned from switch\n");

	if (nTicks == SCHED_NPROC) {
		exit(0);
	}

	unBlock(old_mask);
}

pid_t sched_getfreepid() {
	for (int i = 0; i < SCHED_NPROC; i++) {
		if (waitq.procs[i]->taskAssigned == 0) {
			return waitq.pids[i];
		}
	}
}

void waitq_init() {
	memset(waitq.procs, 0, SCHED_NPROC);
	memset(waitq.pids,0, SCHED_NPROC);
	for (int i = 0; i < SCHED_NPROC; i++) {
		waitq.procs[i] = malloc(sizeof (struct sched_proc));
		waitq.procs[i]->taskAssigned = 0;
		waitq.pids[i] = i+1;
	}
}

void sched_init(void (*init_fn)()) {
	waitq_init();
	signal(SIGVTALRM, sched_tick);

	struct itimerval periodicIntervalTimer;
	periodicIntervalTimer.it_interval.tv_sec = 0;
	periodicIntervalTimer.it_interval.tv_usec = 1000;
	periodicIntervalTimer.it_value.tv_sec = 0;
	periodicIntervalTimer.it_value.tv_usec = 1000;
	setitimer(ITIMER_VIRTUAL, &periodicIntervalTimer, NULL);

	void *newsp;

	if ((newsp=mmap(0,STACK_SIZE,PROT_READ|PROT_WRITE,
		MAP_PRIVATE|MAP_ANONYMOUS,0,0))==MAP_FAILED)
	{
		perror("mmap failed");
	}

	struct savectx ctx;
	errReport(savectx(&ctx),"Failed to save context: ");

	ctx.regs[JB_PC] = init_fn;
	ctx.regs[JB_SP] = newsp + STACK_SIZE;
	ctx.regs[JB_BP] = newsp;

	waitq.procs[0]->stack_basePtr = newsp;
	waitq.procs[0]->stack_stackPtr = newsp + STACK_SIZE;
	waitq.procs[0]->ctx = &ctx;
	waitq.procs[0]->state = SCHED_RUNNING;
	waitq.procs[0]->pid = 1;
	waitq.procs[0]->taskAssigned = 1;
	waitq.procs[0]->ppid = 0;
	waitq.pids[0] = 1;

	current = waitq.procs[0];

	restorectx(&ctx,0);
}

pid_t sched_fork() {
	sigset_t old_mask = block();
	pid_t cpid = sched_getfreepid();
	printf("%d\n",cpid);
	int waitq_index = cpid - 1;

	void * c_sp;

	if ((c_sp=mmap(0,STACK_SIZE,PROT_READ|PROT_WRITE,
		MAP_PRIVATE|MAP_ANONYMOUS,0,0))==MAP_FAILED)
	{
		perror("mmap failed");
	}

	memcpy(c_sp,current->stack_basePtr,STACK_SIZE);

	struct savectx ctx;
	errReport(savectx(&ctx),"Failed to save context: ");

	ctx.regs[JB_SP] = c_sp + STACK_SIZE;
	ctx.regs[JB_BP] = c_sp;

	adjstack(c_sp, c_sp+STACK_SIZE, current->stack_basePtr - (c_sp+STACK_SIZE));

	waitq.procs[waitq_index]->stack_basePtr = c_sp;
	waitq.procs[waitq_index]->stack_stackPtr = c_sp + STACK_SIZE;

	waitq.procs[waitq_index]->ctx = &ctx;
	waitq.procs[waitq_index]->state = SCHED_READY;
	waitq.procs[waitq_index]->pid = cpid;
	waitq.procs[waitq_index]->ppid = current->pid;
	waitq.procs[waitq_index]->taskAssigned = 1;
	waitq.pids[waitq_index] = cpid;
	fprintf(stderr,"forked\n");
	unBlock(old_mask);

	return cpid;
}
