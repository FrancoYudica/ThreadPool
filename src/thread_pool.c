#include <stdio.h>
#include <stdlib.h>
#include "thread_pool.h"


/// @brief Executed by thead, wait for condition to pop next task
void* __thread_task(void* arg)
{
	thpool_t* pool = (thpool_t*)arg;

	THREAD_POOL_LOG("THPOOL: Starting thread %ld\n", pthread_self());
	while(1)
	{
		// Locks, since one thread at the time should be able to
		// access the task queue and wait for the signal.
		pthread_mutex_lock(&pool->queue_mutex);

		// Waits for condition, if there isn't any
		while(task_queue_is_empty(pool->queue) && !pool->shutdown)
		{
			pthread_cond_wait(&pool->thread_task_cond, &pool->queue_mutex);
		}

		if (pool->shutdown)
		{
			THREAD_POOL_LOG("THPOOL: Thread %ld exiting\n", pthread_self());
			pthread_mutex_unlock(&pool->queue_mutex);
			pthread_exit(NULL);
		}
		// THREAD_POOL_LOG("Thread %ld handling the condition\n", pthread_self());

		// Updates the amount of working threads
		pthread_mutex_lock(&pool->working_threads_mutex);
		pool->working_threads++;
		pthread_mutex_unlock(&pool->working_threads_mutex);

		// Pops task of the queue
		thread_task_t task;
		task_arg_t arg;
		task_queue_pop_task(pool->queue, &task, &arg);
		pthread_mutex_unlock(&pool->queue_mutex);

		// Executes task
		task(arg);

		// Updates the amount of working threads
		pthread_mutex_lock(&pool->working_threads_mutex);
		pool->working_threads--;

		if (pool->working_threads == 0 && task_queue_is_empty(pool->queue))
		{
			THREAD_POOL_LOG("THPOOL: All threads finished!\n");
			pthread_cond_broadcast(&pool->cond_all_tasks_done);
		}
		pthread_mutex_unlock(&pool->working_threads_mutex);

	}

	return NULL;
}


/// @brief Creates thread pool, with a fixed amount of threads
thpool_t* thpool_create(int size)
{
	THREAD_POOL_LOG("Initializing thread pool if size %d\n", size);
    thpool_t* pool = (thpool_t*)malloc(sizeof(thpool_t));
	// Sets pool size and threads array
	pool->size = size;
	pool->working_threads = 0;
	pool->threads = (pthread_t*)malloc(size * sizeof(pthread_t));
	pool->shutdown = 0;
	memset(pool->threads, 0, sizeof(pthread_t) * size);

	// Initializes condition
	pthread_cond_init(&pool->thread_task_cond, NULL);
	pthread_cond_init(&pool->cond_all_tasks_done, NULL);
	pthread_mutex_init(&pool->queue_mutex, NULL);
	pthread_mutex_init(&pool->working_threads_mutex, NULL);

	pool->queue = task_queue_create();

	// Creates all threads
	for (int i = 0; i < size; i++)
		pthread_create(&pool->threads[i], NULL, __thread_task, pool);

    return pool;
}

/// @brief Submits a task to the thread pool. If possible, the task will be inmediately executed
void thpool_submit(thpool_t* pool, thread_task_t task, task_arg_t arg)
{
	// Pushes task with argument. 
	// Avoid race condition with mutex
	pthread_mutex_lock(&pool->queue_mutex);
	task_queue_push_task(pool->queue, task, arg);
	pthread_mutex_unlock(&pool->queue_mutex);

	pthread_cond_signal(&pool->thread_task_cond);
}

/// @brief Destroys all the threads and the entire pool
void thpool_destroy(thpool_t* pool)
{
	pthread_mutex_lock(&pool->queue_mutex);

	// Signals all the waiting threads that the pool is destroying
	pool->shutdown = 1;
	pthread_cond_broadcast(&pool->thread_task_cond);

	pthread_mutex_unlock(&pool->queue_mutex);

	for (int i = 0; i < pool->size; i++)
		pthread_join(pool->threads[i], NULL);

	task_queue_destroy(pool->queue);
	pthread_mutex_destroy(&pool->queue_mutex);
	pthread_mutex_destroy(&pool->working_threads_mutex);
	pthread_cond_destroy(&pool->thread_task_cond);
	pthread_cond_destroy(&pool->cond_all_tasks_done);

    free(pool);

	THREAD_POOL_LOG("THPOOL: Thread pool destroyed!\n");
}

int thpool_num_threads_working(thpool_t* pool)
{
	return pool->working_threads;
}

int thpool_all_threads_busy(thpool_t* pool)
{
	return pool->working_threads == pool->size;
}

/// @brief Blocks current thread until all tasks are completed
void thpool_wait_tasks(thpool_t* pool)
{
	pthread_mutex_lock(&pool->working_threads_mutex);

	// Only waits if there are remaining tasks
	if (!task_queue_is_empty(pool->queue))
	{
		pthread_cond_wait(&pool->cond_all_tasks_done, &pool->working_threads_mutex);
	}
	pthread_mutex_unlock(&pool->working_threads_mutex);
}

