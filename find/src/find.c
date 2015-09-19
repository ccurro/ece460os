#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

int pathCombine(const char * dir, const char * base, char * path) {
	strcat(path, dir);
	strcat(path, "/");
	strcat(path, base);

	return 0;
}

int listdir(const char *path) {
	struct dirent *entry;
	DIR *dp;
	char actualpath [PATH_MAX+1];

	realpath(path, actualpath);


	dp = opendir(actualpath);
	if (dp == NULL) {
		perror("opendir");
		return -1;
	}

	char newpath [PATH_MAX+1] ;

	while((entry = readdir(dp))) {
		newpath[0] = '\0';

		char * name = entry->d_name;

		pathCombine(actualpath,name,newpath);

		if (!(strcmp(name,"..") == 0) && !(strcmp(name,".") == 0)) {
			printf(newpath);
			printf("\n");
		}

		if (entry->d_type == DT_DIR && !(strcmp(name,"..") == 0) && !(strcmp(name,".") == 0) ) {

			listdir(newpath); 
		}
	}

	closedir(dp);
	return 0;
}

int main(int argc, char * argv[]) {

	listdir("../..");

	return 0;
}