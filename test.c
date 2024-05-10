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

	thread_pool_t thread_pool;

	printf("Creating pool\n");

	thread_pool_init(&thread_pool, 8);
	printf("Initialized\n");
	char buffer[32];
	for (int i = 0; i < 20; i++)
	{
		thread_pool_submit(&thread_pool, &increment_number, NULL);
	}

	sleep(1);

	thread_pool_wait_tasks(&thread_pool);

	//while(thread_pool_has_remaining_tasks(&thread_pool))
	//{
	//	printf("Didn't finish yet...\n");
	//	sleep(1);
	//}

	thread_pool_destroy(&thread_pool);
	return EXIT_SUCCESS;
}
