#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 5

int shared_value = 0;
int current_turn = 1;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond[NUM_THREADS + 1];

void* thread_func(void* arg) {
    int thread_id = *((int*)arg);

    pthread_mutex_lock(&lock);

    while (current_turn != thread_id) {
        pthread_cond_wait(&cond[thread_id], &lock);
    }

    shared_value++;
    printf("Thread %d: shared_value = %d\n", thread_id, shared_value);

    current_turn++;
    pthread_cond_signal(&cond[current_turn]);

    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_numbers[NUM_THREADS];

    for (int i = 1; i <= NUM_THREADS; i++) {
        pthread_cond_init(&cond[i], NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_numbers[i] = i + 1;
        if (pthread_create(&threads[i], NULL, thread_func, &thread_numbers[i]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    pthread_cond_signal(&cond[1]);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final value of shared_value: %d\n", shared_value);

    for (int i = 1; i <= NUM_THREADS; i++) {
        pthread_cond_destroy(&cond + i);
    }
    pthread_mutex_destroy(&lock);

    return 0;
}
