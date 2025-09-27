#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4

typedef struct {
    int start;
    int end;
} ThreadData;

void* compute_sum(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    double* sum = malloc(sizeof(double));
    *sum = 0.0;
    for (int i = data->start; i <= data->end; i++) {
        *sum += 1.0 / i;
    }
    free(data);
    pthread_exit(sum);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s N\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "N must be positive\n");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[NUM_THREADS];
    int chunk_size = N / NUM_THREADS;
    int remainder = N % NUM_THREADS;
    int start = 1;

    for (int i = 0; i < NUM_THREADS; i++) {
        ThreadData* data = malloc(sizeof(ThreadData));
        data->start = start;
        data->end = start + chunk_size - 1;
        if (i < remainder) {
            data->end += 1;
        }
        start = data->end + 1;

        if (pthread_create(&threads[i], NULL, compute_sum, data) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    double total_sum = 0.0;
    double* partial_sum;
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], (void**)&partial_sum) != 0) {
            fprintf(stderr, "Error joining thread %d\n", i);
            exit(EXIT_FAILURE);
        }
        total_sum += *partial_sum;
        free(partial_sum);
    }

    printf("Total harmonic sum for %d terms = %f\n", N, total_sum);
    return 0;
}
