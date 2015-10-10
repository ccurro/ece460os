// For assignment titled "Fixing your cat"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

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
	uint bufferSize = 256;
	char errStr[256] = "Error: ";

    for (int findex = optind; findex < argc; findex++) {

    	int fd;
    	int nBytesRead = 1;

    	if (strcmp(argv[findex],"-") == 0) {
    		fd = 0;
    	} else {
    		fd = open(argv[findex], O_RDONLY);
	    }

        errReport(fd, argv[findex]);

		unsigned char buffer[bufferSize];

    	while (nBytesRead != 0) {

    		nBytesRead = read(fd,buffer,bufferSize);

            errReport(nBytesRead,argv[findex]);

    		int wCode = write(outFile, buffer, nBytesRead);

            errReport(wCode,"Error on output");
    	}

    	int cCode;

    	if (fd != 0) {
    		cCode = close(fd);
	    }

        errReport(cCode,"Error on close of input: ");
    }

    int cCode;

    if (outFile != STDOUT_FILENO) {
        cCode = close(outFile);
        errReport(cCode, "Error on close of output file");
    }

    return 0;
}