#include "task_queue.h"
#include <stdlib.h>

struct queue_node_t
{
	struct queue_node_t* next;
	thread_task_t task;
	task_arg_t arg;
};

struct task_queue_t
{
	struct queue_node_t* head;
	struct queue_node_t* tail;
	int size;
};


task_queue_t* task_queue_create()
{
	task_queue_t* queue = (task_queue_t*)malloc(sizeof(task_queue_t));
	queue->size = 0;
	queue->head = NULL;
	queue->tail = NULL;
	return queue;
}

/// @brief Pushes task into task_queue_t
void task_queue_push_task(task_queue_t* queue, thread_task_t task, task_arg_t arg)
{
	// Creates next node that stores the task
	struct queue_node_t* next_node = (struct queue_node_t*)malloc(sizeof(struct queue_node_t));
	
	// Sets task
	next_node->task = task;
	next_node->arg = arg;

	queue->size++;

	if (queue->head == NULL)
	{
		// Creates head
		queue->head = next_node;
		queue->tail = next_node;
	}
	else
	{
		// Links to the queue
		queue->tail->next = next_node;

		// Updates tail
		queue->tail = next_node;
	}
}

/// @brief Pops task from task_queue_t
void task_queue_pop_task(task_queue_t* queue, thread_task_t* task, task_arg_t* arg)
{
	// Access top node
	struct queue_node_t* top_node = queue->head;

	// Gets task and argument
	*task = top_node->task;
	*arg = top_node->arg;

	// Changes queue head
	queue->head = queue->head->next;

	free(top_node);

	queue->size--;
}

void task_queue_destroy(task_queue_t *queue)
{

	struct queue_node_t* current = queue->head;
	while(current != NULL)
	{
		struct queue_node_t* next = current->next;
		free(current);
		current = next;
	}
	free(queue);
}

bool task_queue_is_empty(task_queue_t *queue)
{
	return queue->head == NULL;
}
