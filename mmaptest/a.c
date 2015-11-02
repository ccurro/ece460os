// Signal generation for writing to read only mmap.
#include "common.h"

void sigReport(int signo) {
    fprintf(stderr,"Caught SIGSEGV on writing to read only mmap: %s\n", strsignal(signo));
    exit(EXIT_FAILURE);
}

int main() {
	condChk(signal(SIGSEGV, sigReport) == SIG_ERR, "An error occured while setting a signal handler\n");

	int fd = open(FILENAME,O_RDWR);
	errReport(fd,"Failed to open file for testing: ");

	off_t fileSize;
	errReport(getFileSize(fd, &fileSize),"Unable to get size of test file: ");

	char * map = mmap(NULL,(size_t) fileSize, PROT_READ, MAP_SHARED, fd, 0);
	mapChk(map);
	
	map[0] = 1; // will SIGSEGV here

	errReport(munmap(map,(size_t) fileSize),"Failed to unmap map: ");

	errReport(close(fd), "Failed to close file: ");
}