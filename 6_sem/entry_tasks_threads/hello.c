#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 5

void* thread_function(void* arg) {
    int thread_num = *((int*)arg);
    printf("Hello World, I am thread %d, total threads: %d\n", thread_num, NUM_THREADS);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_numbers[NUM_THREADS];
    int i;

    for (i = 0; i < NUM_THREADS; i++) {
        thread_numbers[i] = i + 1;
        int result = pthread_create(&threads[i], NULL, thread_function, &thread_numbers[i]);
        if (result != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
