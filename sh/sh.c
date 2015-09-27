#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>

// Parse string of form :
// command {arg {arg ...} } {redirect {redirect ...}}
int spaceParse(char parsed [][MAX_CANON], char cmdln[MAX_CANON]) {
	// cmdln is a string of the maxium length number of
	// bytes in a terminal canonical input line.

	//parsed in a 2d buffer, one row for each space deliminted item

	int wrd = 0; // wrd of cmd string
	char spc[1];
	strcpy(spc," ");

	for (int i = 0; (cmdln[i] != '\0'); ++i) {
		char c[2];
		c[0] = cmdln[i];

		if (c[0] == spc[0]) {
			wrd++;
		} else {
			strcat(parsed[wrd],c);
		}
	}

	return 0;
}

int redirectParse(char parsed[][MAX_CANON], char cmdln[MAX_CANON]) {
	char spcPrsd[1024][MAX_CANON] = {""};
	spaceParse(spcPrsd, cmdln);

	int nCmd = 0;

	for (int i = 0; (spcPrsd[i][0] != '\0'); ++i) {
		if (strcmp(spcPrsd[i], ">")) {
			strcpy(parsed[nCmd],spcPrsd[i]);
			++nCmd;
		} else {
			strcat(parsed[nCmd],spcPrsd[i]);
		}
	}

}

int main () {
	char cmdln[MAX_CANON]; 
	char prs[1024][MAX_CANON];

	strcpy(cmdln, "quick brown fox > test");

	// spaceParse(prs, cmdln);
	redirectParse(prs,cmdln);

	for (int i = 0; (prs[i][0] != '\0'); ++i) {
		printf("%s\n", prs[i]);
	}

}