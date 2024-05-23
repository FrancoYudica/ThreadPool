#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__
#include <pthread.h>
#include <string.h>
#include "task_queue.h"

#ifdef THPOOL_DEBUG
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
	task_queue_t* queue;

	///@brief Maximum amount of concurrent threads
	int size;

	pthread_t *threads;

	/// @brief Currently working thead count
	int working_threads;

	/// @brief 1 when thread pool is destroying
	int shutdown;
	
} thpool_t;


/// @brief Creates thread pool, with a fixed amount of threads
thpool_t* thpool_create(int size);

/// @brief Submits a task to the thread pool. If possible, the task will be inmediately executed
void thpool_submit(thpool_t* pool, thread_task_t task, task_arg_t arg);

/// @brief Destroys all the threads and the entire pool
void thpool_destroy(thpool_t* pool);

/// @brief Amount of threads that are executing a task
int thpool_num_threads_working(thpool_t* pool);

/// @brief Blocks current thread until all tasks are completed
void thpool_wait_tasks(thpool_t* pool);

int thpool_all_threads_busy(thpool_t* pool);

#endif