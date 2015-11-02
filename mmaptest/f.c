// mmap small file, write beyond end of file in two different pages, check signals
#include "common.h"
#include <setjmp.h>
static jmp_buf buf;

void sigReport(int signo) {
    fprintf(stderr,"Caught SIGBUS on writing to mmap: %s\n", strsignal(signo));
    longjmp(buf,1); 
}

int main() {
	condChk(signal(SIGBUS, sigReport) == SIG_ERR, "An error occured while setting a signal handler\n");

	int fd = open("smallfile",O_RDWR);
	errReport(fd,"Failed to open file for testing: ");

	char * map = mmap(NULL,(size_t) 16384, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	mapChk(map);

	if (!setjmp(buf)) {
		map[8191] = 'e';
	} else {
		map[8193] = 'f';
	}

	// On the second time around the signal handler does not get called.
	// Instead the program just core dumps with a bus error.

	errReport(close(fd),"Failed to close file: ");
}