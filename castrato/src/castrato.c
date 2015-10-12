// For assignment titled "Fixing your cat"
#define _GNU_SOURCE 
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

int errReport(int returnVal, char * errStr) {
    if (returnVal == -1) {
        fprintf(stderr,"Error: %s ", errStr);
        perror("");
        exit(-1);
    }
}

int main(int argc, char * argv[]) {
	int c;
	int outFile = STDOUT_FILENO;
	uint bufferSize = 1024;
	char errStr[256] = "Error: ";

    for (int findex = optind; findex < argc; findex++) {

        int more_pipe[2];
        int grep_pipe[2];
        pid_t more_pid;
        pid_t grep_pid;

        if (pipe(more_pipe) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        if (pipe(grep_pipe) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        more_pid = fork();

        if (more_pid == 0) {
            dup2(more_pipe[0],STDIN_FILENO);
            close(more_pipe[0]);
            close(more_pipe[1]);
            close(grep_pipe[0]);
            close(grep_pipe[1]);
            execvp("more",NULL);
            // fprintf(stderr, "Failed to open more: %s\n", strerror(errno));
            // exit(EXIT_FAILURE);
            // execlp("less","less",stdin, (char *) NULL);
        } 

        grep_pid = fork();

        if (grep_pid == 0) {
            //exec to grep
            dup2(grep_pipe[0],STDIN_FILENO);
            dup2(more_pipe[1],STDOUT_FILENO);
            close(more_pipe[0]);
            close(more_pipe[1]);
            close(grep_pipe[0]);
            close(grep_pipe[1]);

            char * argv[3];
            argv[0] = "grep";
            argv[1] = "exec";
            argv[2] = NULL;
            execlp("grep","grep","exec", (char *) NULL);
        }


    	int fd;
    	int nBytesRead = 1;
		fd = open(argv[findex], O_RDONLY);

        errReport(fd, argv[findex]);

		unsigned char buffer[bufferSize];

    	while (nBytesRead != 0) {

    		nBytesRead = read(fd,buffer,bufferSize);

            errReport(nBytesRead,argv[findex]);

    		int wCode = write(more_pipe[1], buffer, nBytesRead);

            errReport(wCode,"Error on output");
    	}

    	int cCode;
		cCode = close(fd);

        errReport(cCode,"Error on close of input: ");

        wait(0);
        wait(0);
    }

    return 0;
}
