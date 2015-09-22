#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>

int pathCombine(const char * dir, const char * base, char * path) {
	strcat(path, dir);
	strcat(path, "/");
	strcat(path, base);

	return 0;
}

int statPerms(struct stat fileStat)
{

	printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
	printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
	printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
	printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
	printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
	printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
	printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
	printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
	printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
	printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");

	return 0;
}


int listdir(const char *path, int uid, int mtm, int count) {
	struct dirent *entry;
	DIR *dp;
	char actualpath [PATH_MAX+1];

	realpath(path, actualpath);

	dp = opendir(actualpath);
	if (dp == NULL) {
		perror("opendir");
		return -1;
	}

	char newpath [PATH_MAX+1];
	char linkres [PATH_MAX+1];
	struct stat sb; 
	struct passwd * userInfo;
	struct group  * grpInfo;
	struct tm * tmInfo;
	char * tmStr;

	while((entry = readdir(dp))) {
		newpath[0] = '\0';
		linkres[0] ='\0';

		char * name = entry->d_name;

		pathCombine(actualpath,name,newpath);

		lstat(newpath,&sb);

		time_t now = time(0);

		int A = (mtm == 0);
		int B = (now - sb.st_mtime) > abs(mtm);
		int C = (mtm > 0);
		// Modify Time logic is A + !(B xor C)

		if (!(strcmp(name,"..") == 0) && !(strcmp(name,".") == 0) && ((sb.st_uid == uid) || (uid == -1)) && (A || (B == C))) {
			printf("%d/%d %d ",sb.st_dev,sb.st_ino,sb.st_nlink);
			statPerms(sb);
			printf(" ");
			userInfo = getpwuid(sb.st_uid);
			printf(userInfo->pw_name);
			printf(" ");
			grpInfo = getgrgid(sb.st_gid);
			printf(grpInfo->gr_name);
			if ((sb.st_mode & S_IFBLK) || (sb.st_mode & S_IFCHR)) {
				printf(" BLK/CHR "); // print raw device number
			} else {
				printf(" %d ",sb.st_size);
			}
			tmInfo = localtime(&sb.st_mtime);
			tmStr = asctime(tmInfo);
			tmStr[24] = 0;
			printf(tmStr);
			printf(" ");
			printf(newpath);
			if (S_ISLNK(sb.st_mode))
			{
				printf(" -> ");
				readlink(newpath,linkres,PATH_MAX+1);
				char * respath = realpath(linkres,NULL);
				printf(respath);
			}

			printf("\n");
		}

		if (entry->d_type == DT_DIR && !(strcmp(name,"..") == 0) && !(strcmp(name,".") == 0) ) {
			count++;
			if (count == 1024) {
				printf("Maximum depth reached");
				return -1;
			}
			listdir(newpath,uid,mtm,count); 
		}
	}

	closedir(dp);
	return 0;
}

int main(int argc, char * argv[]) {

	int mtm = 0;
	int uid = -1;
	char username[32];
	int c;
	int count = 0;
	int userFilter = 0;

	while ( (c = getopt(argc, argv, "u:m:")) != -1) {
		switch (c) {
			case 'u':
				sscanf(optarg, "%d", &uid);
				sscanf(optarg, "%s", &username);
				userFilter = 1;
				break;
			case 'm':
				sscanf(optarg, "%d", &mtm);
				break;
			case '?':
				break;
				default:
			printf ("?? getopt returned character code 0%o ??\n", c);
		}
	}

	char searchpath[PATH_MAX+1];

	sscanf(argv[optind],"%s",searchpath);

	if ((uid == -1) && (userFilter ==1 )) {
		struct passwd * uidInfo; 
		uidInfo = getpwnam(username);
		uid = uidInfo->pw_uid;
	}

	listdir(searchpath, uid,mtm,count); 

	return 0;

}
