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

int main (int argc, char * argv[]) {

	char scriptName[MAX_CANON] = {""};
	char * line[MAX_CANON];
	size_t len = 0;

	if (argc > 2) {
		printf("Two many input arguments\n");
		return -1;
	}

	strcat(scriptName,argv[1]); // load shell script.

	FILE * script = fopen(scriptName, "r");

	char * wrds[MAX_CANON];
	char * token;

	while (getline(line, &len, script) != -1) {

		char * ln = strtok(*line,"\n"); // remove trailing newline.
		int i = 0;
		int cmt = 0;
		int Nstderr = 0;
		int Nstdin  = 0;
		int Nstdout = 0;
		int rStdin  = 0;
		int rStdout = 0;
		int rStderr = 0;

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
				rStdout = open(token,O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
				Nstdout++;
			} else if (strcmp(token,">>") == 0) {
				token = strtok(NULL, " ");
				rStdout = open(token,O_APPEND | O CREAT | O WRONLY, S_IRUSR | S_IWUSR);
				Nstdout++;
			} else if (strcmp(token,"2>") == 0) {
				token = strtok(NULL, " ");
				rStderr = open(token,O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
				Nstderr++;
			} else if (strcmp(token,"2>>") == 0) {
				token = strtok(NULL, " ");
				rStderr = open(token,O_APPEND | O CREAT | O WRONLY, S_IRUSR | S_IWUSR);
				Nstderr++;
			}
			else {	
				wrds[i] = strdup(token);
				i++;
			}
			token = strtok(NULL, " ");
		}

		if ((Nstdout < 2) && (Nstderr < 2) && (Nstdin < 2) != 1) {
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

		c_pid = fork(); //duplicate                                                                                                                                                

		if( c_pid == 0 ){
			if (rStdin)
				dup2(rStdin,STDIN_FILENO);

			if (rStdout) 
				dup2(rStdout,STDOUT_FILENO);

			if (rStderr) 
				dup2(rStderr,STDERR_FILENO);

			fcloseall(); // give clean file descriptor env.
			execvp(cmd,sargv);
		} else if (c_pid > 0){
			int stat_val = 0;

			wait(&stat_val);

			while (WIFEXITED(stat_val) == 0) {
				wait(&stat_val);
			}
		} else {
			perror("fork failed");
			_exit(2); //exit failure, hard 
		}
	}

	return 0;
}