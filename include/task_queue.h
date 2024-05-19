#ifndef __TASK_STACK_H__
#define __TASK_STACK_H__
#include <stdbool.h>


typedef void(*thread_task_t)(void*);  // Defines a function pointer type for tasks
typedef void* task_arg_t;             // Defines a type for task arguments

// Forward declaration to hide implementation details
typedef struct task_queue_t task_queue_t;

/**
 * @brief Creates a new task queue.
 *
 * This function allocates and initializes a new task queue.
 * It should be called to create a task queue before any operations
 * are performed on it.
 *
 * @return Pointer to the newly created task queue.
 */
task_queue_t* task_queue_create();


/// @brief Pushes a task into the task queue.
/// 
/// This function adds a new task to the queue. The task is specified
/// by a function pointer and an argument, which will be passed to the 
/// function when the task is executed.
/// 
/// @param queue Pointer to the task queue.
/// @param task Function pointer representing the task to be executed.
/// @param arg Argument to be passed to the task function.
void task_queue_push_task(task_queue_t* queue, thread_task_t task, task_arg_t arg);

/// @brief Pops a task from the task queue.
/// 
/// This function removes and retrieves the next task from the queue. If
/// the queue is empty, the behavior of this function is undefined.
/// 
/// @param queue Pointer to the task queue.
/// @param task Output parameter that will receive the function pointer of the task.
/// @param arg Output parameter that will receive the argument to be passed to the task function.
void task_queue_pop_task(task_queue_t* queue, thread_task_t* task, task_arg_t* arg);

/**
 * @brief Destroys the task queue.
 *
 * This function deallocates and cleans up any resources associated with the task queue.
 * It should be called when the task queue is no longer needed to avoid memory leaks.
 *
 * @param queue Pointer to the task queue to destroy.
 */
void task_queue_destroy(task_queue_t* queue);

/**
 * @brief Checks if the task queue is empty.
 *
 * This function determines whether the task queue is empty or not.
 *
 * @param queue Pointer to the task queue to check.
 * @return true if the task queue is empty, false otherwise.
 */
bool task_queue_is_empty(task_queue_t* queue);


#endif