#include "common.h"
#include "sched.h"

#define STACK_SIZE 65536

// struct sched_proc * current;
int nTicks = 0;
sched_waitq waitq;
struct savectx fork_ctx;
sigset_t old_mask;

void sched_switch() {
	fprintf(stderr,"In switching routine\n");
	for (int i = 0; i < SCHED_NPROC; i++) {
		if (waitq.procs[i]->state == SCHED_READY) {
			fprintf(stderr,"pid %d, is in state %d\n",i+1,waitq.procs[i]->state);
			if (current->state == SCHED_RUNNING) {
				current->state = SCHED_READY;
			}

			waitq.procs[i]->state = SCHED_RUNNING;

			current = waitq.procs[i];
			fprintf(stderr,"Switched to pid %d\n", waitq.pids[i]);
			unBlock(old_mask);
			restorectx(waitq.procs[i]->ctx,1);
		}
	}
	printf("All zombies\n");
	exit(0);
}

void sched_tick(int signo) {
	old_mask = block();
	nTicks++;	
	fprintf(stderr,"\t tick # %d\n",nTicks);

	if (current->state != SCHED_WAITING) {
		current->state = SCHED_READY;
	}

	sched_ps();

	int jmpd = savectx(current->ctx);

	if (!jmpd) {
		sched_switch();
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
		waitq.procs[i]->nLivingChildren = 0;
		waitq.pids[i] = i+1;
	}
}

void sched_init(void (*init_fn)()) {
	signal(SIGSEGV, sigsegvHandler);	
	waitq_init();
	signal(SIGVTALRM, sched_tick);

	struct itimerval periodicIntervalTimer;
	periodicIntervalTimer.it_interval.tv_sec = 0;
	periodicIntervalTimer.it_interval.tv_usec = 1000;
	// periodicIntervalTimer.it_interval.tv_usec = 10000;
	periodicIntervalTimer.it_value.tv_sec = 0;
	periodicIntervalTimer.it_value.tv_usec = 1000;
	// periodicIntervalTimer.it_value.tv_usec = 10000;
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
	printf("Next pid %d\n",cpid);

	if(savectx(current->ctx)){
		unBlock(old_mask); 		
		return cpid;
	} else {

		int waitq_index = cpid - 1;
		sched_proc parent = *current;

		void * stack_basePtr;

		if ((stack_basePtr = mmap(0,STACK_SIZE,PROT_READ|PROT_WRITE,
			MAP_PRIVATE|MAP_ANONYMOUS,0,0))==MAP_FAILED)
		{
			perror("mmap failed");
		}

		for (int i = 0; i < STACK_SIZE; i++) {
			// fprintf(stderr,"%d ",i);
			*((char*) (stack_basePtr + i));
			*((char*) (parent.stack_basePtr + i));			
		}

		fprintf(stderr,"Made it through loop\n");

		memcpy(stack_basePtr,parent.stack_basePtr,STACK_SIZE);
		struct savectx ctx;

		int jmped;
		jmped = savectx(&ctx);

		if (jmped) {
			fprintf(stderr,"Return to child\n");
			return 0;
		}

		fprintf(stderr,"Manipulating stack\n");

		int diff = stack_basePtr - parent.stack_basePtr;

		ctx.regs[JB_SP] += diff;
		ctx.regs[JB_BP] += diff;

		waitq.procs[waitq_index]->stack_basePtr += diff;
		waitq.procs[waitq_index]->stack_stackPtr += diff;

		adjstack(stack_basePtr, stack_basePtr + STACK_SIZE, diff);	

		waitq.procs[waitq_index]->ctx = &ctx;
		waitq.procs[waitq_index]->state = SCHED_READY;
		waitq.procs[waitq_index]->pid = cpid;
		waitq.procs[waitq_index]->ppid = parent.pid;
		waitq.procs[waitq_index]->taskAssigned = 1;
		waitq.pids[waitq_index] = cpid;

		fprintf(stderr,"forked to make pid %d\n",cpid);

		sched_switch();
	}
}

void sched_exit(int code) {
	fprintf(stderr,"Exited\n");
	waitq.procs[current->ppid-1]->nLivingChildren--;
	old_mask = block();	
	current->state = SCHED_ZOMBIE;	
	sched_switch();
}

void sched_ps() {
	printf("\npid \t state\n");
	for (int i = 0; i < SCHED_NPROC; i++) {
		if (waitq.procs[i]->taskAssigned) {
			printf("%d \t %d\n",waitq.procs[i]->pid,waitq.procs[i]->state);
		}
	}
	printf("\n");
}
void sigsegvHandler(int signo) {
	sched_ps();
	fprintf(stderr,"SIGSEGV\n");
	exit(0);
}
