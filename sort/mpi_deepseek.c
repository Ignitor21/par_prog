#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int* read_array_from_file(const char* filename, int* out_size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return NULL;
    }

    int count = 0;
    int temp;
    while (fscanf(file, "%d", &temp) == 1) {
        count++;
    }

    if (count == 0) {
        fclose(file);
        return NULL;
    }

    rewind(file);
    int* array = (int*)malloc(count * sizeof(int));
    if (array == NULL) {
        fclose(file);
        perror("Memory allocation failed");
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        if (fscanf(file, "%d", &array[i]) != 1) {
            free(array);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    *out_size = count;
    return array;
}

void is_sorted(const int* arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            printf("Массив не отсортирован\n");
            return;
        }
    }
    printf("Массив отсортирован\n");
}

int compare_asc(const void *a, const void *b) { return (*(int*)a - *(int*)b); }
int compare_desc(const void *a, const void *b) { return (*(int*)b - *(int*)a); }

void compare_exchange(int *local_data, int size, int partner, int direction, int rank, int num_procs) {
    if (partner < 0 || partner >= num_procs) return;

    int *partner_data = (int *)malloc(size * sizeof(int));
    int *merged = (int *)malloc(2 * size * sizeof(int));
    MPI_Status status;

    MPI_Sendrecv(local_data, size, MPI_INT, partner, 0,
                partner_data, size, MPI_INT, partner, 0,
                MPI_COMM_WORLD, &status);

    // Проверка на усечение
    int received;
    MPI_Get_count(&status, MPI_INT, &received);
    if (received != size) {
        printf("Process %d: Received %d elements, expected %d\n", rank, received, size);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (int i = 0; i < size; i++) {
        merged[i] = local_data[i];
        merged[size + i] = partner_data[i];
    }

    qsort(merged, 2 * size, sizeof(int), direction ? compare_asc : compare_desc);

    if (rank < partner) {
        for (int i = 0; i < size; i++) local_data[i] = merged[i];
    } else {
        for (int i = 0; i < size; i++) local_data[i] = merged[size + i];
    }

    free(partner_data);
    free(merged);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int N = 0;
    int *global_data = NULL;

    if (rank == 0) {
        global_data = read_array_from_file("input.txt", &N);
        if (!global_data) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        is_sorted(global_data, N);
    }

    // Рассылаем размер массива всем процессам
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Проверка делимости
    if (N % num_procs != 0) {
        if (rank == 0) {
            printf("Error: Array size %d is not divisible by %d processes\n", N, num_procs);
        }
        MPI_Finalize();
        return 1;
    }

    int local_size = N / num_procs;
    int *local_data = (int *)malloc(local_size * sizeof(int));
    if (!local_data) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Scatter(global_data, local_size, MPI_INT,
               local_data, local_size, MPI_INT,
               0, MPI_COMM_WORLD);

    qsort(local_data, local_size, sizeof(int), compare_asc);

    for (int k = 2; k <= num_procs; k *= 2) {
        for (int j = k / 2; j > 0; j /= 2) {
            int partner = rank ^ j;
            int direction = ((rank & k) == 0);
            compare_exchange(local_data, local_size, partner, direction, rank, num_procs);
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }

    MPI_Gather(local_data, local_size, MPI_INT,
              global_data, local_size, MPI_INT,
              0, MPI_COMM_WORLD);

    if (rank == 0) {
        is_sorted(global_data, N);
        free(global_data);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}
