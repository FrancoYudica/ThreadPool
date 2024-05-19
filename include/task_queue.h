#ifndef __TASK_STACK_H__
#define __TASK_STACK_H__

typedef void(*thread_task_t)(void*);
typedef void* task_arg_t;

struct __queue_node_t
{
	struct __queue_node_t* next;
	thread_task_t task;
	task_arg_t arg;
};

typedef struct
{
	struct __queue_node_t* head;
	struct __queue_node_t* tail;
	int size;
} task_queue_t;

void task_queue_init(task_queue_t* queue);

/// @brief Pushes task into task_queue_t
void task_queue_push_task(task_queue_t* queue, thread_task_t task, task_arg_t arg);
/// @brief Pops task from task_queue_t
void task_queue_pop_task(task_queue_t* queue, thread_task_t* task, task_arg_t* arg);

void task_queue_clear(task_queue_t* queue);

#endif