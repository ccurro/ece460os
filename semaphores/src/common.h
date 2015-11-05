#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/mman.h>

#define NPROCS	64

void condChk(int condition, char * str);
void errReport(int returnVal, char * errStr);
void mapChk(int * map);
void spinLock(volatile int * lck);
int getFileSize(int fd, off_t * fileSize);
int tas (volatile int * lock);