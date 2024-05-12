#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "thread_pool.h"


void increment_number(void* arg)
{
	char* character = (char*)arg;
	printf("Thread %ld solving task... Character: %c\n", pthread_self(), *character);
}

int main()
{
	thpool_t* thread_pool = thpool_init(8);

	char message[] = "This is a THPOOL test!";
	for (int i = 0; i < strlen(message); i++)
	{
		thpool_submit(thread_pool, &increment_number, &message[i]);
	}

	printf("Waiting tasks to finish...\n");
	thpool_wait_tasks(thread_pool);

	thpool_destroy(thread_pool);
	return EXIT_SUCCESS;
}
