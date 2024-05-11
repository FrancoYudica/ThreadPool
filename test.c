#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "thread_pool.h"


void increment_number(void* arg)
{
	static int n = 0;
	sleep(3);
	printf("Awake! number: %d\n", n++);
}

int main()
{


	printf("Creating pool\n");

	thpool_t* thread_pool = thpool_init(8);
	printf("Initialized\n");
	char buffer[32];
	for (int i = 0; i < 20; i++)
	{
		thpool_submit(thread_pool, &increment_number, NULL);
	}

	sleep(1);

	thpool_wait_tasks(thread_pool);

	thpool_destroy(thread_pool);
	return EXIT_SUCCESS;
}
