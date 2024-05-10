#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__
#include <pthread.h>
#include <string.h>
#include "task_queue.h"

#define THREAD_POOL_DEBUG

#ifdef THREAD_POOL_DEBUG

#define THREAD_POOL_LOG(...) printf(__VA_ARGS__);

#else
#define THREAD_POOL_LOG(...)
#endif


typedef struct
{
	/// @brief Mutex used to push and pop tasks in the queue
	pthread_mutex_t queue_mutex;

	/// @brief Mutex used to modify the amount of currenlty working theads
	pthread_mutex_t working_threads_mutex;

	/// @brief Condition used to signal when a new task is added
	pthread_cond_t thread_task_cond;

	/// @brief Condition signaled when all tasks are completed
	pthread_cond_t cond_all_tasks_done;

	/// @brief Queue that holds tasks
	task_queue_t queue;

	///@brief Maximum amount of concurrent threads
	int size;

	pthread_t *threads;

	/// @brief Currently working thead count
	int working_threads;

	/// @brief 1 when thread pool is destroying
	int shutdown;
	
} thread_pool_t;

void* __thread_task(void* arg);

/// @brief Initializes thread pool, with a fixed amount of threads
void thread_pool_init(thread_pool_t* pool, int size)
{
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

	task_queue_init(&pool->queue);

	// Creates all threads
	for (int i = 0; i < size; i++)
		pthread_create(&pool->threads[i], NULL, __thread_task, pool);
}

/// @brief Submits a task to the thread pool. If possible, the task will be inmediately executed
void thread_pool_submit(thread_pool_t* pool, thread_task_t task, task_arg_t arg)
{
	// Pushes task with argument. 
	// Avoid race condition with mutex
	pthread_mutex_lock(&pool->queue_mutex);
	task_queue_push_task(&pool->queue, task, arg);
	pthread_mutex_unlock(&pool->queue_mutex);

	pthread_cond_signal(&pool->thread_task_cond);
}

/// @brief Destroys all the threads and the entire pool
void thread_pool_destroy(thread_pool_t* pool)
{
	pthread_mutex_lock(&pool->queue_mutex);

	// Signals all the waiting threads that the pool is destroying
	pool->shutdown = 1;
	pthread_cond_broadcast(&pool->thread_task_cond);

	pthread_mutex_unlock(&pool->queue_mutex);

	for (int i = 0; i < pool->size; i++)
		pthread_join(pool->threads[i], NULL);

	task_queue_clear(&pool->queue);
	pthread_mutex_destroy(&pool->queue_mutex);
	pthread_mutex_destroy(&pool->working_threads_mutex);
	pthread_cond_destroy(&pool->thread_task_cond);
	pthread_cond_destroy(&pool->cond_all_tasks_done);

	THREAD_POOL_LOG("Thread pool destroyed!\n", pthread_self());
}

int thread_pool_has_remaining_tasks(thread_pool_t* pool)
{
	return pool->working_threads > 0;
}

/// @brief Blocks current thread until all tasks are completed
void thread_pool_wait_tasks(thread_pool_t* pool)
{
	pthread_mutex_lock(&pool->working_threads_mutex);
	pthread_cond_wait(&pool->cond_all_tasks_done, &pool->working_threads_mutex);
	pthread_mutex_unlock(&pool->working_threads_mutex);
}

/// @brief Executed by thead, wait for condition to pop next task
void* __thread_task(void* arg)
{
	thread_pool_t* pool = (thread_pool_t*)arg;

	THREAD_POOL_LOG("Starting thread %ld\n", pthread_self());
	while(1)
	{
		// Locks, since one thread at the time should be able to
		// access the task queue and wait for the signal.
		pthread_mutex_lock(&pool->queue_mutex);

		// Waits for condition, if there isn't any
		while(pool->queue.size == 0 && !pool->shutdown)
		{
			pthread_cond_wait(&pool->thread_task_cond, &pool->queue_mutex);
		}

		if (pool->shutdown)
		{
			THREAD_POOL_LOG("Thread %ld exiting\n", pthread_self());
			pthread_mutex_unlock(&pool->queue_mutex);
			pthread_exit(NULL);
		}
		THREAD_POOL_LOG("Thread %ld handling the condition\n", pthread_self());

		// Updates the amount of working threads
		pthread_mutex_lock(&pool->working_threads_mutex);
		pool->working_threads++;
		pthread_mutex_unlock(&pool->working_threads_mutex);

		// Pops task of the queue
		thread_task_t task;
		task_arg_t arg;
		task_queue_pop_task(&pool->queue, &task, &arg);
		pthread_mutex_unlock(&pool->queue_mutex);

		// Executes task
		task(arg);

		// Updates the amount of working threads
		pthread_mutex_lock(&pool->working_threads_mutex);
		pool->working_threads--;

		if (pool->working_threads == 0 && pool->queue.size == 0)
		{
			pthread_cond_broadcast(&pool->cond_all_tasks_done);
		}
		pthread_mutex_unlock(&pool->working_threads_mutex);

	}

	return NULL;
}

#endif