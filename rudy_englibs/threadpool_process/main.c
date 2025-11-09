#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "lib/rudy_threadpool.h"

void* air_data_sensing(void* arg) {
    int task_id = (int)(intptr_t)arg;
    sleep(2);
    printf("Type: 1\t haha\tThread ID: %lu == > Task %d ==> Execution Complete\n===> Handle Air Data Sensing", (unsigned long)pthread_self(), task_id);
    return NULL;
}

void* flight_control(void* arg) {
    int task_id = (int)(intptr_t)arg;
    sleep(2);
    printf("Type: 2\t hihi\tThread ID: %lu == > Task %d ==> Execution Complete\n===> Handle Flight Control", (unsigned long)pthread_self(), task_id);
    return NULL;
}

void* navigation(void* arg) {
    int task_id = (int)(intptr_t)arg;
    sleep(2);
    printf("Type: 3\t huhu\tThread ID: %lu == > Task %d ==> Execution Complete\n===> Handle Navigation", (unsigned long)pthread_self(), task_id);
    return NULL;
}


int main() {

    thread_pool_t pool;
    THREADPOOL_init(&pool, 63, 123);

    worker_func_t task_types[] = {air_data_sensing, flight_control, navigation};

    const int total_tasks = 128;
    uint64_t task_ids[total_tasks];
    for (int i = 0; i < total_tasks; i++) {
        uint64_t task_id = pool.add_task(&pool, task_types[i % 3], (void *)(intptr_t)i);
        if (task_id == 0) {
            fprintf(stderr, "Failed to add task %d\n", i);
        }
        task_ids[i] = task_id;
        printf("Added Task %d with ID %llu\n", i, task_id);
    }
    // loop checking all task states
    sleep(4);
    for (int i = 0; i < total_tasks; i++) {
        task_state_enum_t state = pool.get_task_state(&pool, task_ids[i]);
        printf("State of Task ID %llu: %s\n", task_ids[i], pool
            .task_state_to_string(&pool,state));
    }

    printf("Destroying thread pool...\n");
    pool.destroy(&pool);
    printf("Thread pool destroyed. Exiting program.\n");
    return 0;
}
