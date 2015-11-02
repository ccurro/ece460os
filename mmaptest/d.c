// Open with MAP_SHARED, write beyond end of file, check for file size increase;
#include "common.h"

int main() {
	int fd = open("nonPagedfile",O_RDWR);
	errReport(fd,"Failed to open file for testing: ");

	off_t fileSize;
	errReport(getFileSize(fd, &fileSize),"Unable to get size of test file: ");

	char * map = mmap(NULL,(size_t) fileSize + 1, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	mapChk(map);

	map[fileSize+1] = 'd';

	errReport(munmap(map,(size_t) fileSize + 1),"Failed to unmap map: ");

	off_t newFileSize;
	errReport(getFileSize(fd, &newFileSize),"Unable to get size of test file: ");

	if (fileSize == newFileSize) {
		printf("When a write is made one byte beyond the size of an mmapped file, for a file with a size that is not a multiple of of the page size, its size as reported by stat(2) does not change.\n");
	} else {
		printf("When a write is made one byte beyond the size of an mmapped file, for a file with a size that is not a multiple of of the page size, its size as reported by stat(2) changes.\n");
	}

	errReport(close(fd),"Failed to close file: ");
}