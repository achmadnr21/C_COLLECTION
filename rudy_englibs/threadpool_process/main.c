#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "lib/rudy_threadpool.h"

void* my_task_function(void* arg) {
    int task_id = (int)(intptr_t)arg;
    sleep(2);
    printf("Thread ID: %lu == > Task %d ==> Execution Complete\n", (unsigned long)pthread_self(), task_id);
    return NULL;
}


int main() {

    thread_pool_t pool;
    THREADPOOL_init(&pool, 5, 40);

    // Add tasks to the pool
    
    const int total_tasks = 20;
    uint64_t task_ids[total_tasks];
    for (int i = 0; i < total_tasks; i++) {
        uint64_t task_id = pool.add_task(&pool, my_task_function, (void *)(intptr_t)i);
        if (task_id == 0) {
            fprintf(stderr, "Failed to add task %d\n", i);
        }
        task_ids[i] = task_id;
        printf("Added Task %d with ID %llu\n", i, task_id);
    }
    // loop checking all task states
    sleep(3);
    for (int i = 0; i < total_tasks; i++) {
        task_state_enum_t state = pool.get_task_state(&pool, task_ids[i]);
        printf("State of Task ID %llu: %s\n", task_ids[i], pool
            .task_state_to_string(&pool,state));
    }
    // Destroy the pool
    pool.destroy(&pool);
    return 0;
}
