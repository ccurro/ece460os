#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>		

void empty() {}

int timeval_subtract (struct timespec * result, struct timespec * x, struct timespec * y) {
	if (x->tv_nsec < y->tv_nsec) {
		int psec = (y->tv_nsec - x->tv_nsec) / 1000000000 + 1;
		y->tv_nsec -= 1000000000 * psec;
		y->tv_sec += psec;
	}
	if (x->tv_nsec - y->tv_nsec > 1000000000) {
		int psec = (x->tv_nsec - y->tv_nsec) / 1000000000;
		y->tv_nsec += 1000000000 * psec;
		y->tv_sec -= psec;
	}

	result->tv_sec = 	x->tv_sec - y->tv_sec;
	result->tv_nsec = x->tv_nsec - y->tv_nsec;

	return x->tv_sec < y->tv_sec;
}

int test(void (*handler)(void)) {
	struct timespec start;
	struct timespec end;
	int nTimes = 40000000;

	if (handler == NULL) {
		clock_gettime(CLOCK_REALTIME,&start);
		for (int i = 0; i < nTimes; i++) {
			//Will not get optimized out b/c of -O0 flag
		}
		clock_gettime(CLOCK_REALTIME,&end);
	} else {
		clock_gettime(CLOCK_REALTIME,&start);
		for (int i = 0; i < nTimes; i++) {
			handler();
		}
		clock_gettime(CLOCK_REALTIME,&end);
	}

	struct timespec result;
	timeval_subtract(&result, &end, &start);

	return (10000000000*result.tv_sec + result.tv_nsec)/nTimes;
}

int main() {
	int emptyLoop = test(NULL);
	printf("%-29s %d (ns)\n","Empty loop iteration: ", emptyLoop);
	
	int emptyFunc = test(empty) - emptyLoop;
	printf("Empty function call in loop:  %d (ns)\n",emptyFunc);

	// cast output of getuid() to be void * rather than int
	int t = test((void *) getuid) - emptyLoop - emptyFunc; 
	printf("%-28s %d (ns)\n","Syscall (getuid) in loop: ", t);
}