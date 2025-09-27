#ifndef PID_H
#define PID_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

typedef void* (*worker_func_t)(void*);

typedef struct _task{
    worker_func_t func;
    void* arg;
} task_t;

typedef struct _thread_pool {
    pthread_t* threads;        // Array of worker threads
    size_t thread_count;       // Number of threads in the pool
    size_t task_queue_size;    // Size of the task queue
    size_t task_count;         // Number of tasks in the queue
    pthread_mutex_t mutex;     // Mutex for synchronizing access to the task queue
    pthread_cond_t cond;       // Condition variable for signaling tasks
    task_t* task_queue;       // Queue of tasks to be processed

    void (*push)(struct _thread_pool* pool, worker_func_t func, void* arg);
    void (*submit)(struct _thread_pool* pool, worker_func_t func, void* arg);
    void (*destroy)(struct _thread_pool* pool);
} thread_pool_t;
void THREADPOOL_init(thread_pool_t* pool, size_t thread_count, size_t task_queue_size);

#endif