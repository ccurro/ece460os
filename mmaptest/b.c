// Open with MAP_SHARED, then write to mapped memory.
// Check if update is visable using read(2)
#include "common.h"

int main() {
	int fd = open(FILENAME,O_RDWR);
	errReport(fd,"Failed to open file for testing: ");

	off_t fileSize;
	errReport(getFileSize(fd, &fileSize),"Unable to get size of test file: ");

	char * map = mmap(NULL,(size_t) fileSize, PROT_WRITE, MAP_SHARED, fd, 0);
	mapChk(map);

	char writeStr[] = "The rule is, jam to-morrow and jam yesterday - but never jam to-day.";
	int buffSize = sizeof writeStr;

	for (int i = 0; i < buffSize ; i++) {
		map[i] = writeStr[i];
	}

	char buff[buffSize];

	errReport(read(fd, buff, buffSize),"Failed to read from test file: ");

	if (strcmp(buff,writeStr) == 0) {
		printf("Update to an mmapped file with MAP_SHARED is visible to read(2).\n");
	} else {
		printf("Update to an mmapped file with MAP_SHARED is not visible to read(2).\n");
	}

	errReport(close(fd),"Failed to close file: ");
}