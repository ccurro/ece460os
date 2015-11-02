// Open with MAP_SHARED, write beyond end of file, then write(2) to generative "hole"
// Check previoulsy written bytes.
#include "common.h"

int main() {
	int fd = open("nonPagedfile",O_RDWR);
	errReport(fd,"Failed to open file for testing: ");

	off_t fileSize;
	errReport(getFileSize(fd, &fileSize),"Unable to get size of test file: ");

	char * map = mmap(NULL,(size_t) fileSize+1, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	mapChk(map);

	map[fileSize+1] = 'e';

	errReport(lseek(fd, fileSize + 2, SEEK_SET),"Failed to lseek(2) past end of file: ");

	char writeStr[] = "The vorpal blade went snicker-snack!";

	errReport(write(fd, writeStr, sizeof writeStr),"Failed to write(2) past end of file: ");

	errReport(lseek(fd, fileSize+1, SEEK_SET),"Failed to lseek(2) past end of file: ");

	char buff[2];
	errReport(read(fd, buff, 1),"Failed to read(2) past end of file: ");

	if (buff[0] = map[fileSize+1]) {
		printf("If we create a \"hole\" in a file, any changes previously made in an mmapped region beyond the end of the file will be visible.\n");
	} else {
		printf("If we create a \"hole\" in a file, any changes previously made in an mmapped region beyond the end of the file will not be visible.\n");
	}

	errReport(munmap(map,(size_t) fileSize + 1),"Failed to unmap map: ");
	errReport(close(fd),"Failed to close file: ");
}