// Test for mutex protection with spinLock
#include "common.h"

int main() {
    int * map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0 );
    // MAP_AYNOYMOUS not available on all systems
    mapChk(map);
    map[0] = 0;
    int * lock = &map[1];

    pid_t pids[NPROCS];

    for (int i = 0; i < NPROCS; i++) {
        pids[i] = fork();
        errReport(pids[i],"Failed to fork: ");
        if (pids[i] == 0) {
            spinLock(lock);

            for (int j = 0; j < 100000; j++) {
                map[0]++;
            }

            *lock = 0;
            exit(0);
        }
    }

    for (int i = 0; i < NPROCS; i++) {
        wait(0);
    }

    printf("%d\n",map[0]);
}