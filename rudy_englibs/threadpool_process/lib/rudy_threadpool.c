#include "rudy_threadpool.h"

// ================================= THREAD POOL TASK PROTOTYPES ==============================
static uint64_t          TASK_enqueue(task_queue_t* queue, worker_func_t func, void* arg);
static task_t*           TASK_dequeue(task_queue_t* queue);
static uint8_t           TASK_purge(task_queue_t* queue);
static uint8_t           TASK_is_empty(task_queue_t* queue);
static uint8_t           TASK_is_full(task_queue_t* queue);
static uint8_t           TASK_destroy(task_queue_t* queue);
static task_state_enum_t TASK_get_state(task_queue_t* queue, uint64_t task_id);
static void              THREADPOOL_TASK_init(task_queue_t* queue, size_t capacity);
// ================================= THREAD POOL TASK IMPLEMENTATION ==============================

void THREADPOOL_TASK_init(task_queue_t* queue, size_t capacity) {
    queue->tasks    = (task_t*)malloc(sizeof(task_t) * capacity);
    queue->capacity = capacity;
    queue->next_id  = 1;

    queue->stop = 0;

    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond_not_empty, NULL);
    pthread_cond_init(&queue->cond_not_full, NULL);
    pthread_cond_init(&queue->cond_stop, NULL);

    for (size_t i = 0; i < capacity; i++) {
        queue->tasks[i].state = TASK_STATE_OPEN;
    }

    queue->enqueue   = TASK_enqueue;
    queue->dequeue   = TASK_dequeue;
    queue->purge     = TASK_purge;
    queue->is_empty  = TASK_is_empty;
    queue->is_full   = TASK_is_full;
    queue->destroy   = TASK_destroy;
    queue->get_state = TASK_get_state; // assign state getter
}

uint8_t TASK_is_empty(task_queue_t* queue) {
    // Check if there are any waiting or running tasks
    // perform butterfly check
    size_t half_capacity = queue->capacity / 2;
    for (size_t i = 0; i < half_capacity; i++) {
        if (queue->tasks[i].state == TASK_STATE_WAITING ||
            queue->tasks[i].state == TASK_STATE_RUNNING) {
            return 0;
        }
        size_t rev_index = queue->capacity - 1 - i;
        if (queue->tasks[rev_index].state == TASK_STATE_WAITING ||
            queue->tasks[rev_index].state == TASK_STATE_RUNNING) {
            return 0;
        }
    }
    return 1;
}

uint8_t TASK_is_full(task_queue_t* queue) {
    // Count active tasks (waiting + running)
    size_t active_tasks  = 0;
    size_t half_capacity = queue->capacity / 2;
    for (size_t i = 0; i < half_capacity; i++) {
        if (queue->tasks[i].state == TASK_STATE_WAITING ||
            queue->tasks[i].state == TASK_STATE_RUNNING) {
            active_tasks++;
        }
        size_t rev_index = queue->capacity - 1 - i;
        if (queue->tasks[rev_index].state == TASK_STATE_WAITING ||
            queue->tasks[rev_index].state == TASK_STATE_RUNNING) {
            active_tasks++;
        }
    }
    return active_tasks >= queue->capacity;
}

uint8_t TASK_destroy(task_queue_t* queue) {
    if (!queue)
        return 1;

    pthread_mutex_lock(&queue->mutex);

    // Free the task array
    free(queue->tasks);
    queue->tasks = NULL;

    // Reset other members
    queue->capacity = 0;
    queue->next_id  = 0;

    pthread_mutex_unlock(&queue->mutex);
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond_not_empty);
    pthread_cond_destroy(&queue->cond_not_full);

    return 0;
}

uint64_t TASK_enqueue(task_queue_t* queue, worker_func_t func, void* arg) {
    pthread_mutex_lock(&queue->mutex);

    size_t active_tasks = 0;
    for (size_t i = 0; i < queue->capacity; i++) {
        if (queue->tasks[i].state == TASK_STATE_WAITING ||
            queue->tasks[i].state == TASK_STATE_RUNNING) {
            active_tasks++;
        }
    }

    while (active_tasks >= queue->capacity) {
        pthread_cond_wait(&queue->cond_not_full, &queue->mutex);
        active_tasks = 0;
        for (size_t i = 0; i < queue->capacity; i++) {
            if (queue->tasks[i].state == TASK_STATE_WAITING ||
                queue->tasks[i].state == TASK_STATE_RUNNING) {
                active_tasks++;
            }
        }
    }

    size_t slot_index = 0;
    for (size_t i = 0; i < queue->capacity; i++) {
        if (queue->tasks[i].state != TASK_STATE_WAITING &&
            queue->tasks[i].state != TASK_STATE_RUNNING) {
            slot_index = i;
            break;
        }
    }

    uint64_t task_id                          = queue->next_id++;
    queue->tasks[slot_index].id               = task_id;
    queue->tasks[slot_index].func             = func;
    queue->tasks[slot_index].arg              = arg;
    queue->tasks[slot_index].state            = TASK_STATE_WAITING;
    queue->tasks[slot_index].worker_thread_id = 0;

    pthread_cond_signal(&queue->cond_not_empty);
    pthread_mutex_unlock(&queue->mutex);
    return task_id;
}

task_t* TASK_dequeue(task_queue_t* queue) {
    pthread_mutex_lock(&queue->mutex);

    while (!queue->stop) {
        // Find a waiting task and immediately mark it as running
        for (size_t i = 0; i < queue->capacity; i++) {
            if (queue->tasks[i].state == TASK_STATE_WAITING) {
                queue->tasks[i].state            = TASK_STATE_RUNNING;
                queue->tasks[i].worker_thread_id = pthread_self();
                pthread_mutex_unlock(&queue->mutex);
                return &queue->tasks[i];
            }
        }

        // No waiting tasks found, wait for signal
        pthread_cond_wait(&queue->cond_not_empty, &queue->mutex);
    }

    pthread_mutex_unlock(&queue->mutex);
    return NULL;
}

uint8_t TASK_purge(task_queue_t* queue) {
    pthread_mutex_lock(&queue->mutex);
    pthread_cond_broadcast(&queue->cond_not_full);
    pthread_cond_broadcast(&queue->cond_not_empty);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

task_state_enum_t TASK_get_state(task_queue_t* queue, uint64_t task_id) {
    pthread_mutex_lock(&queue->mutex);
    for (size_t i = 0; i < queue->capacity; i++) {
        if (queue->tasks[i].id == task_id) {
            task_state_enum_t state = queue->tasks[i].state;
            pthread_mutex_unlock(&queue->mutex);
            return state;
        }
    }
    pthread_mutex_unlock(&queue->mutex);
    return -1; // Return OPEN if task not found
}

// ================================= THREAD POOL PROTOTYPES ==============================
static void*             THREADPOOL_worker(void* arg);
static uint64_t          THREADPOOL_add_task(thread_pool_t* pool, worker_func_t func, void* arg);
static task_state_enum_t THREADPOOL_get_task_state(thread_pool_t* pool, uint64_t task_id);
static char*   THREADPOOL_task_state_to_string(struct _thread_pool* pool, task_state_enum_t state);
static uint8_t THREADPOOL_destroy(thread_pool_t* pool);
// ================================= THREAD POOL IMPLEMENTATION ==============================
void THREADPOOL_init(thread_pool_t* pool, size_t thread_size, size_t task_queue_size) {
    if (thread_size & 1) {
        printf("Adjusted thread size to even number\n -> from %zu into %zu\n", thread_size,
               thread_size + 1);
        thread_size++;
    }
    if (task_queue_size & 1) {
        printf("Adjusted task queue size to even number\n -> from %zu into %zu\n", task_queue_size,
               task_queue_size + 1);
        task_queue_size++;
    }

    THREADPOOL_TASK_init(&pool->task_queue, task_queue_size);
    pool->threads = (pthread_t*)malloc(sizeof(pthread_t) * thread_size);
    pool->count   = thread_size;
    // Set public API function pointers
    pool->add_task             = THREADPOOL_add_task;
    pool->destroy              = THREADPOOL_destroy;
    pool->get_task_state       = THREADPOOL_get_task_state;
    pool->task_state_to_string = THREADPOOL_task_state_to_string;

    for (size_t i = 0; i < thread_size; i++) {
        pthread_create(&pool->threads[i], NULL, THREADPOOL_worker, pool);
    }
    pool->is_running = 1;
}

void* THREADPOOL_worker(void* arg) {
    thread_pool_t* pool = (thread_pool_t*)arg;

    while (1) {
        task_t* task = pool->task_queue.dequeue(&pool->task_queue);
        if (!task || !task->func) // Check if task is NULL or invalid
            break;

        // Task is already marked as RUNNING and worker_thread_id is set in dequeue
        // Execute the task function
        task->func(task->arg);

        // Mark task as completed and signal availability
        pthread_mutex_lock(&pool->task_queue.mutex);
        task->state = TASK_STATE_COMPLETED;
        pthread_cond_signal(&pool->task_queue.cond_not_full);
        pthread_mutex_unlock(&pool->task_queue.mutex);
    }
    return NULL;
}

uint64_t THREADPOOL_add_task(thread_pool_t* pool, worker_func_t func, void* arg) {
    if (!pool->is_running)
        return 0;

    return pool->task_queue.enqueue(&pool->task_queue, func, arg);
}

task_state_enum_t THREADPOOL_get_task_state(thread_pool_t* pool, uint64_t task_id) {
    return pool->task_queue.get_state(&pool->task_queue, task_id);
}

uint8_t THREADPOOL_destroy(thread_pool_t* pool) {
    if (!pool->is_running)
        return 1;

    pool->is_running = 0;

    pthread_mutex_lock(&pool->task_queue.mutex);

    while (!pool->task_queue.is_empty(&pool->task_queue)) {
        pthread_cond_wait(&pool->task_queue.cond_not_full, &pool->task_queue.mutex);
    }

    pool->task_queue.stop = 1;
    pthread_cond_broadcast(&pool->task_queue.cond_not_empty);
    pthread_mutex_unlock(&pool->task_queue.mutex);

    for (size_t i = 0; i < pool->count; i++)
        pthread_join(pool->threads[i], NULL);

    free(pool->threads);
    pool->task_queue.destroy(&pool->task_queue);

    return 0;
}

// ================================= THREAD POOL HELPER ==============================
char* THREADPOOL_task_state_to_string(struct _thread_pool* pool, task_state_enum_t state) {
    switch (state) {
    case TASK_STATE_OPEN:
        return "OPEN";
    case TASK_STATE_WAITING:
        return "WAITING";
    case TASK_STATE_RUNNING:
        return "RUNNING";
    case TASK_STATE_COMPLETED:
        return "COMPLETED";
    case TASK_STATE_CANCELED:
        return "CANCELED";
    default:
        return "ALREADY NOT IN TASK";
    }
}
