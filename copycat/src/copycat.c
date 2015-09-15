#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
	int c;
	int outFile = STDOUT_FILENO;
	uint bufferSize = 256;
	char errStr[256] = "Error: ";

	while ( (c = getopt(argc, argv, "b:o:")) != -1) {
		switch (c) {
			case 'b':
        		sscanf(optarg, "%u", &bufferSize); // - '0';
        		break;
        	case 'o':
        		if( access( optarg, F_OK ) != -1 ) {
        			remove(optarg);
        		}
        		outFile = open(optarg, O_CREAT | O_RDWR);
        		break;
        	case '?':
        		break;
        	default:
        		printf ("?? getopt returned character code 0%o ??\n", c);
        }
    }

    for (int findex = optind; findex < argc; findex++) {

    	int fd;
    	int nBytesRead = 1;

    	if (strcmp(argv[findex],"-") == 0) {
    		fd = 0;
    	} else {
    		fd = open(argv[findex], O_RDONLY);
	    }

    	if (fd == -1) {
    		perror(strcat(errStr, argv[findex]));
    		return -1;
    	}

		unsigned char buffer[bufferSize];

    	while (nBytesRead != 0) {

    		nBytesRead = read(fd,buffer,bufferSize);

    		if (nBytesRead == -1) {
    			perror(strcat(errStr, argv[findex]));
    			return -1;
    		}

    		int wCode = write(outFile, buffer, nBytesRead);

    		if (wCode == -1) {
    			perror("Error on output: ");
    			return -1;
    		}
    	}

    	int cCode;

    	if (fd != 0) {
    		cCode = close(fd);
	    }

    	if (cCode == -1) {
    		perror("Error on close: ");
    		return -1;
    	}
    }	

    return 0;
}