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

void spinLock(volatile int * lck) {
	while(tas(lck) != 0) ;
}

void unLock(volatile int * lock) {
	*lock = 0;
}