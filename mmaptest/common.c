#include "common.h"

void condChk(int condition, char * str) {
    if (condition) {
        fprintf(stderr,str);
        exit(EXIT_FAILURE);
    }
}

void mapChk(char * map) {
	char errStr[1024];
    sprintf(errStr, "%s%s \n", "Failed to mmap file: ",strerror(errno));
	condChk(map == MAP_FAILED, errStr);
}

void errReport(int returnVal, char * errStr) {
    char str[1024];
    sprintf(str, "%s%s \n", errStr,strerror(errno));
    condChk(returnVal == -1, str);
}

int getFileSize(int fd, off_t * fileSize) {
	struct stat fileStat;
	if (fstat(fd,&fileStat) == -1) {
		return -1;
	} else {
		*fileSize = fileStat.st_size;
		return 0;
	}
}