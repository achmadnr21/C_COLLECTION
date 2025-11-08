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
    printf("Thread ID: %lu\nExecuting Task %d\n", (unsigned long)pthread_self(), task_id);
    pdelay(1);
    printf("Finished task %d\n\n", task_id);
    return NULL;
}


int main() {

    thread_pool_t pool;
    THREADPOOL_init(
        &pool,
        4,
        10);

    // Add tasks to the pool
    for (int i = 0; i < 20; i++) {
        uint64_t task_id = pool.add_task(&pool, my_task_function, (void *)(intptr_t)i);
        if (task_id == 0) {
            fprintf(stderr, "Failed to add task %d\n", i);
        }
        printf("Added Task %d with ID %llu\n", i, task_id);
    }

    // Destroy the pool
    pool.destroy(&pool);
    return 0;
}
