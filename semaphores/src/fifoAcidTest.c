// Create a shared fifo, fork writers, fork reader, show that for many writers,
// each writing more than the buffer size, the reader recieves all bytes in the
// (relative) correct order.
#include "common.h"
#include "sem.h"
#include "fifo.h"

int main() {	
	struct fifo f;
	fifo_init(&f);

	int nWriters = 63;
	int writeLength = 8192; // Twice the fifo buffer size
	pid_t pids[nWriters];

	for (int i = 0; i < nWriters; i++) {
		pids[i] = fork();
		errReport(pids[i], "Failed to fork writer: ");
		
		if (pids[i] == 0) {
			my_procnum = i;
			unsigned long writeBuf[writeLength];

			for (int j = 0; j <writeLength; j++) {
				writeBuf[j] = j + getpid()*10000;
			}

			fifo_wr(&f, writeBuf, writeLength);
			exit(0);
		}
	}

	pid_t reader = fork();
	errReport(reader,"Failed to fork reader: ");

	if (reader == 0) {
		my_procnum = nWriters; // one procnum higher that last writer
		unsigned long readBuf[nWriters*writeLength];
		int rd = fifo_rd(&f, readBuf, nWriters*writeLength);

		for (int i = 0; i < rd; i++) {
			printf("%d\n",readBuf[i]);
		}

		exit(0);
	}

	for (int i = 0; i < nWriters + 1; i++) {
		wait(0);
	}
}
