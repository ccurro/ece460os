// For assignment titled "Fixing your cat"
#define _GNU_SOURCE 
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

int totalBytes = 0;
int totalFiles = 0;

void byteReport(int signo) {
    fprintf(stderr,"%s, Bytes read: %d, Files read: %d \n", strsignal(signo), totalBytes, totalFiles);
}

void condChk(int condition, char * str) {
    if (condition) {
        fprintf(stderr,str);
        exit(EXIT_FAILURE);
    }
}

int errReport(int returnVal, char * errStr) {
    char str[1024];
    sprintf(str, "%s, %s ", errStr,strerror(errno));
    condChk(returnVal == -1, str);
}

void cpipe( int p[2]) {
    for (int i = 0; i < 2; i++)
        errReport(close(p[i]),"Error on pipe close, ");
}

int main(int argc, char * argv[]) {
    condChk(signal(SIGINT, byteReport) == SIG_ERR, "An error occured while setting a signal handler");
    condChk(argc < 3, "Too few input arguments.\n ex: castrato pattern infile1 [...infile2...]\n");

	uint bufferSize = 1024;

    for (int findex = 2; findex < argc; findex++) {

        int more_pipe[2], grep_pipe[2];
        pid_t more_pid, grep_pid;

        // options set so I don't have to close pipes manually in children
        errReport(pipe2(more_pipe,O_CLOEXEC), "Error on pipe open, ");
        errReport(pipe2(grep_pipe,O_CLOEXEC), "Error on pipe open, ");

        int fd = open(argv[findex], O_RDONLY); 
        errReport(fd, argv[findex]);
        totalFiles++;

        more_pid = fork();

        if (more_pid == 0) {
            errReport(dup2(more_pipe[0],STDIN_FILENO),"Error on dup2, ");
            execvp("more",NULL);
            fprintf(stderr, "Failed to open more: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        } else if (more_pid == -1) {
            fprintf(stderr, "Failed to open more: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        grep_pid = fork();

        if (grep_pid == 0) {
            errReport(dup2(grep_pipe[0],STDIN_FILENO),"Error on dup2, ");
            errReport(dup2(more_pipe[1],STDOUT_FILENO),"Error on dup2, ");;
            execlp("grep","grep",argv[1], (char *) NULL);
            fprintf(stderr, "Failed to open grep: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        } else if (grep_pid == -1) {
            fprintf(stderr, "Failed to open grep: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

    	int nBytesRead = 1;
        unsigned char buffer[bufferSize];

    	while (nBytesRead != 0) {
    		nBytesRead = read(fd,buffer,bufferSize);
            errReport(nBytesRead,argv[findex]);
            errReport(write(grep_pipe[1], buffer, nBytesRead),"Error on output");
            totalBytes = totalBytes + nBytesRead;
    	}

        errReport(close(fd),"Error on close of input: ");

        cpipe(more_pipe);
        cpipe(grep_pipe);
        wait(0); // one wait for each child
        wait(0);
    }

    return 0;
}