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
#include <sys/time.h>
#include "savectx64.h"

#define NPROCS	64

void condChk(int condition, char * str);
void errReport(int returnVal, char * errStr);
void mapChk(void * map);
sigset_t block();
void unBlock(sigset_t oldmask);

#endif