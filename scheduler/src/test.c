#include "common.h"
#include "sched.h"

void init_fn() {
	printf("Hello World!\n");

	int arr[2];
	arr[0] = 31415;
	arr[1] = 21718;
	printf("%p\n",arr);

	sched_fork();
	printf("Hello\n");
	printf("%p\n",arr);	
	while (1) {

	}
}

int main() {
	sched_init(init_fn);
}