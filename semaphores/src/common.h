#ifndef COMMON_H
#define COMMON_H

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
void mapChk(void * map);
void spinLock(volatile int * lck);
void unLock(volatile int * lock);
int tas(volatile int * lock);

#endif