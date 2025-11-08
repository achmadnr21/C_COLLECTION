#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "lib/rudy_threadpool.h"

void pdelay(int seconds) {
    clock_t start_time = clock();
    while (clock() < start_time + seconds * CLOCKS_PER_SEC);
}

void* my_task_function(void* arg) {
    int task_id = (int)(intptr_t)arg;
    pdelay(20);
    printf("Thread ID: %lu\nExecuting Task %d\n", (unsigned long)pthread_self(), task_id);
    return NULL;
}


int main() {

    thread_pool_t pool;
    THREADPOOL_init(
        &pool,
        32,
        40);

    // Add tasks to the pool
    uint64_t first_task_id = 0;
    for (int i = 0; i < 20; i++) {
        uint64_t task_id = pool.add_task(&pool, my_task_function, (void *)(intptr_t)i);
        if(i == 0) {
            first_task_id = task_id;
        }
        if (task_id == 0) {
            fprintf(stderr, "Failed to add task %d\n", i);
        }
        printf("Added Task %d with ID %llu\n", i, task_id);
    }

    printf("\nChecking state of first task (ID: %llu):\n", first_task_id+1);
    task_state_enum_t state = pool.get_task_state(&pool, first_task_id+1);
    printf("State of Task ID %llu: %s\n", first_task_id+1, pool.task_state_to_string(&pool,state));

    // Destroy the pool
    pool.destroy(&pool);
    return 0;
}
