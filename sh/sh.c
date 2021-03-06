#define _GNU_SOURCE 
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>

void mydup(int fd1, int fd2) {
	if (fd1) {
		if (dup2(fd1,fd2) < 0) {
			char str[20];
			sprintf(str, "Error on redirect of file desc %d", fd2);
			perror(str);
		    exit(EXIT_FAILURE);
		}
	}
}

int main (int argc, char * argv[]) {

	int returnval = 0;
	char scriptName[MAX_CANON] = {""};
	char * line[MAX_CANON];
	size_t len = 0;
	FILE * script;

	if (argc > 2) {
		printf("Two many input arguments\n");
		return -1;
	} else if (argc == 2) {
		strcat(scriptName,argv[1]); // load shell script.
		script = fopen(scriptName, "r");
	} else {
		script = fdopen(STDIN_FILENO,"r");
		printf("$ ");
	}

	char * wrds[MAX_CANON];
	char * token;

	while (getline(line, &len, script) != -1) {

		char * ln = strtok(*line,"\n"); // remove trailing newline.
		int i = 0;
		int cmt = 0;
		int Nstderr = 0, Nstdin  = 0, Nstdout = 0;
		int rStdin  = 0, rStdout = 0, rStderr = 0;

		token = strtok(ln, " ");

		while( token != NULL ) {
			if (token[0] == '#') {
				cmt = 1;
				break; // handle comment lines
			}
			if (strcmp(token,"<") == 0) {
				token = strtok(NULL, " ");
				rStdin = open(token,O_RDONLY);
				Nstdin++;
			} else if (strcmp(token,">") == 0) {
				token = strtok(NULL, " ");
				rStdout = open(token,O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
				Nstdout++;
			} else if (strcmp(token,">>") == 0) {
				token = strtok(NULL, " ");
				rStdout = open(token,O_APPEND | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
				Nstdout++;
			} else if (strcmp(token,"2>") == 0) {
				token = strtok(NULL, " ");
				rStderr = open(token,O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
				Nstderr++;
			} else if (strcmp(token,"2>>") == 0) {
				token = strtok(NULL, " ");
				rStderr = open(token,O_APPEND | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
				Nstderr++;
			}
			else {	
				wrds[i] = strdup(token);
				i++;
			}
			token = strtok(NULL, " ");
		}

		if (!
			((Nstdout < 2) && (Nstderr < 2) && (Nstdin < 2))) {
			printf("Too many redirects on a given file descriptor\n");
			return -1;
		}

		if (cmt)
			continue;

		char * cmd = wrds[0];
		char * sargv[i+1];

		sargv[0] = cmd;
		sargv[i+1] = NULL;

		for (int j = 1; j < i; j++)
			sargv[j] = wrds[j];

		pid_t c_pid;

		c_pid = fork();

		if( c_pid == 0 ){
			mydup(rStdin,STDIN_FILENO);
			mydup(rStdout,STDOUT_FILENO);
			mydup(rStderr,STDERR_FILENO);

			fcloseall(); // give clean file descriptor env.
			execvp(cmd,sargv);
		} else if (c_pid > 0){
			int stat_val = 0;

			struct rusage cusage;
			wait3(&stat_val, 0, &cusage);
			printf("sys:  %ld.%06ld\n", (long int)(cusage.ru_stime.tv_sec), (long int)(cusage.ru_stime.tv_usec));
			printf("user: %ld.%06ld\n", (long int)(cusage.ru_utime.tv_sec), (long int)(cusage.ru_utime.tv_usec));
			printf("Exit Status: %d\n\n", WEXITSTATUS(stat_val));

			returnval += stat_val;	

		} else {
			perror("fork failed");
			_exit(2);
		}
		if (argc == 1)
			printf("\n$ ");
	}

	return returnval;
}