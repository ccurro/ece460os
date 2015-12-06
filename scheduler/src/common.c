#include "common.h"

void condChk(int condition, char * str) {
	if (condition) {
		fprintf(stderr,str);
		exit(EXIT_FAILURE);
	}
}

void mapChk(void * map) {
	char errStr[1024];
	sprintf(errStr, "%s%s \n", "Failed to mmap file: ",strerror(errno));
	condChk(map == MAP_FAILED, errStr);
}

void errReport(int returnVal, char * errStr) {
	char str[1024];
	sprintf(str, "%s%s \n", errStr,strerror(errno));
	condChk(returnVal == -1, str);
}

sigset_t block() {
	sigset_t block_mask;
	sigset_t old_mask;
	// Block all signals for critical region
	sigfillset(&block_mask);
	sigdelset(&block_mask,SIGINT);
	sigprocmask(SIG_SETMASK,&block_mask, &old_mask);
	return old_mask;
}

void unBlock(sigset_t old_mask) {
	sigprocmask(SIG_SETMASK,&old_mask,NULL);
}