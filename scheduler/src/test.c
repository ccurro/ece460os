#include "common.h"
#include "sched.h"
#include <time.h>

void waitFor (unsigned int secs) {
    unsigned int retTime = time(0) + secs;     // Get finishing time.
    while (time(0) < retTime);    // Loop until it arrives.
}

void init_fn() {
	int nChildren = 300;
	int s = 0;
	for (int i = 0; i < nChildren; i++) {
		if (sched_fork() != 0) {
		// in parent
			current->nLivingChildren++;
			printf("In Parent\n");
			printf("\tpid: %d\n",current->pid);		
			printf("\t%p\n",current->stack_basePtr);					

		} else {
			printf("Should be in child\n");
			printf("\tpid: %d\n",current->pid);
			printf("\t%p\n",current->stack_basePtr);
			s = s + 1;
			printf("sum = %d\n",s);
			// waitFor(3);
			// if (sched_fork() != 0) {
			// // in child
			// 	printf("In Child\n");
			// 	printf("%p\n",current->stack_basePtr);					
			// 	sched_exit(0);
			// } else {
			// 	printf("Should be in grandchild\n");
			// 	printf("%p\n",current->stack_basePtr);

			// 	sched_exit(0);		
			// }
			sched_exit(0);		
		}
	}
	current->state = SCHED_WAITING;
	while(current->nLivingChildren > 0) {
		printf("Waiting\n");
	}
	sched_exit(0);	
}

int main() {
	sched_init(init_fn);
}