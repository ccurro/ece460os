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

int inode_list[1024]; 
char dir_list[1024][PATH_MAX];
int inode_list_count = 0;
// only count inodes of dirs and can only open 1024 dirs
// (list for loop detection)

int elof(int val, int *arr, int size){
    int i;
    for (i=0; i < size; i++) {
        if (arr[i] == val)
            return 1;
    }
    return 0;
}

int inodeConflict(ino_t inode, const char *path, int * inode_list, int count) {
	if (count == 0) {
		inode_list[count] = inode;
		strcpy(dir_list[count],path);
		return 0;
	} else {
		if (elof(inode, inode_list, count) == 1) {
			return 1;
		} else {
			inode_list[count] = inode;
			strcpy(dir_list[count],path);
			return 0;
		}
	}
}

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


int listdir(const char *path, int uid, int mtm, int vol, int count) {
	struct dirent *entry;
	DIR *dp;
	char actualpath [PATH_MAX+1];

	realpath(path, actualpath);

	dp = opendir(actualpath);
	if (dp == NULL) {
		// perror("opendir");
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
			printf("%lld/%lld %d ",sb.st_dev,sb.st_ino,sb.st_nlink);
			statPerms(sb);
			printf(" ");
			userInfo = getpwuid(sb.st_uid);
			if (userInfo) {
				printf(userInfo->pw_name);
				printf(" ");
			} else {
				printf("%d ",sb.st_uid);
			}
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

			if ((inodeConflict(sb.st_ino, newpath, inode_list, inode_list_count) == 0) && ((vol == 0) || (vol == sb.st_dev))) {
				listdir(newpath,uid,mtm,vol,count); 
				inode_list_count++;
			} else if (vol != sb.st_dev) {
				printf("Not crossing mount point at");
				printf(actualpath);
				printf("\n");
			}
			else {
				printf("Hit conflict: ");
				printf(newpath);
				printf(", ");
				printf(dir_list[inode_list_count-1]);
				printf("\n");
			}
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
	int sameVol = 0;
	int vol = 0;

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
			case 'x':
				sameVol = 1;
				break;
			case '?':
				break;
				default:
			printf ("?? getopt returned character code 0%o ??\n", c);
		}
	}

	char searchpath[PATH_MAX+1];

	sscanf(argv[optind],"%s",searchpath);

	if (sameVol == 1) {
		struct stat sp;
		// DIR * pd = opendir(realpath(searchpath,NULL));
		// struct dirent * entry = readdir(pd);
		lstat(realpath(searchpath,NULL),&sp);
		vol = sp.st_dev;
	}

	if ((uid == -1) && (userFilter ==1 )) {
		struct passwd * uidInfo; 
		uidInfo = getpwnam(username);
		uid = uidInfo->pw_uid;
	}

	listdir(searchpath,uid,mtm,vol,count); 

	return 0;

}
