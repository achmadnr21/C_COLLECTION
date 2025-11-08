#ifndef RUDY_THREADPOOL_H
#define RUDY_THREADPOOL_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

typedef void *(*worker_func_t)(void *);

typedef enum {
    TASK_STATE_WAITING,
    TASK_STATE_RUNNING,
    TASK_STATE_COMPLETED,
    TASK_STATE_CANCELED
} task_state_enum_t;

typedef struct _task
{
    uint64_t id;
    worker_func_t func;
    void *arg;

    pthread_t worker_thread_id;
    task_state_enum_t state;

} task_t;

typedef struct _task_queue
{
    task_t *tasks;    // Array of tasks
    size_t capacity;  // Maximum number of tasks
    size_t count;     // Current number of tasks
    size_t head;      // Index of the task to be processed
    size_t tail;      // Index of latest added task
    uint64_t next_id; // ID for the next task
    pthread_mutex_t mutex; // Mutex for thread safety
    pthread_cond_t cond_not_empty; // Condition variable for not empty
    pthread_cond_t cond_not_full; // Condition variable for not full

    uint8_t stop;                // Flag to indicate stopping the queue
    pthread_cond_t cond_stop;    // Function pointers for task queue operations

    uint64_t (*enqueue)(struct _task_queue *queue, worker_func_t func, void *arg); // Function to add a task
    task_t* (*dequeue)(struct _task_queue *queue);                                  // Function to remove a task
    uint8_t (*purge)(struct _task_queue *queue);                                   // Function to clear all tasks, we will send that current condition is full until all tasks are done
    uint8_t (*is_empty)(struct _task_queue *queue);                                // Check if the queue is empty
    uint8_t (*is_full)(struct _task_queue *queue);                                 // Check if the queue is full
    uint8_t (*destroy)(struct _task_queue *queue);                                 // Destroy the task queue
    task_state_enum_t (*get_state)(struct _task_queue *queue, uint64_t task_id);  // Get task state
} task_queue_t;

typedef struct _thread_pool
{
    pthread_t *threads; // Array of worker threads
    size_t count;       // Number of threads
    uint8_t is_running; // Pool running status

    task_queue_t task_queue; // Task queue

    uint64_t (*add_task)(struct _thread_pool *pool, worker_func_t func, void *arg); // Function to add a task
    task_state_enum_t (*get_task_state)(struct _thread_pool *pool, uint64_t task_id); // Function to get task state
    char* (*task_state_to_string)(struct _thread_pool *pool, task_state_enum_t state); // Function to convert task state to string
    uint8_t (*destroy)(struct _thread_pool *pool);                                 // Function to destroy the thread pool
} thread_pool_t;

void THREADPOOL_init(thread_pool_t *pool, size_t thread_count, size_t task_queue_size);

#endif // RUDY_THREADPOOL_H