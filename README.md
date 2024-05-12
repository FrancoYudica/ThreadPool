# Thread Pool

## Overview

This project implements a thread pool in C, providing a flexible and efficient way to manage concurrent tasks.

## Features

- **Thread Pool Initialization**: Initialize a thread pool with a fixed number of threads.
- **Task Submission**: Submit tasks to the thread pool for execution.
- **Task Execution**: Tasks are executed concurrently by available threads in the pool.
- **Thread Pool Destruction**: Cleanly destroy the thread pool when it's no longer needed.
- **Thread Status Monitoring**: Check the number of threads currently executing tasks.
- **Task Completion Synchronization**: Wait for all submitted tasks to complete before proceeding.

## How to Use

### Prerequisites

- Make sure you have a C compiler installed on your Unix/Linux system (e.g., GCC).
- This project relies on POSIX threads (pthread), so it is compatible only with Unix/Linux systems.


### Building the Project

1. Clone this repository to your local machine.
2. Navigate to the project directory.
3. Compile the project using your preferred C compiler. For example:
   ```bash
   gcc -o test test.c thread_pool.c task_queue.c -D THPOOL_DEBUG -pthread
