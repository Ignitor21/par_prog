#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int* read_array_from_file(const char* filename, int* out_size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return NULL;
    }

    // Первый проход: подсчет количества чисел
    int count = 0;
    int temp;
    while (fscanf(file, "%d", &temp) == 1) {
        count++;
    }

    if (count == 0) {
        fclose(file);
        return NULL;
    }

    // Возвращаемся в начало файла
    rewind(file);

    // Выделение памяти
    int* array = (int*)malloc(count * sizeof(int));
    if (array == NULL) {
        fclose(file);
        perror("Memory allocation failed");
        return NULL;
    }

    // Второй проход: чтение чисел
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
    return;
}

// Компараторы для qsort
int compare_asc(const void *a, const void *b) { return (*(int*)a - *(int*)b); }
int compare_desc(const void *a, const void *b) { return (*(int*)b - *(int*)a); }

// Функция для слияния и разделения данных между партнёрами
void compare_exchange(int *local_data, int size, int partner, int direction, int rank) {
    int *partner_data = (int *)malloc(size * sizeof(int));
    int *merged = (int *)malloc(2 * size * sizeof(int));
    MPI_Status status;

    // Обмен данными с партнёром
    MPI_Sendrecv(local_data, size, MPI_INT, partner, 0,
                partner_data, size, MPI_INT, partner, 0,
                MPI_COMM_WORLD, &status);

    // Слияние данных
    for (int i = 0; i < size; i++) {
        merged[i] = local_data[i];
        merged[size + i] = partner_data[i];
    }

    // Сортировка в нужном направлении
    if (direction == 1) {
        qsort(merged, 2 * size, sizeof(int), compare_asc);
    } else {
        qsort(merged, 2 * size, sizeof(int), compare_desc);
    }

    // Сохранение нужной половины
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
        is_sorted(global_data, N);
    }

    int local_size = N / num_procs;
    int *local_data = (int *)malloc(local_size * sizeof(int));

    MPI_Scatter(global_data, local_size, MPI_INT, 
               local_data, local_size, MPI_INT, 
               0, MPI_COMM_WORLD);

    // Локальная сортировка
    qsort(local_data, local_size, sizeof(int), compare_asc);

    // Основной алгоритм битонической сортировки
    for (int k = 2; k <= num_procs; k *= 2) {
        for (int j = k / 2; j > 0; j /= 2) {
            int partner = rank ^ j;
            if (partner >= num_procs) continue;
            
            int direction = ((rank & k) == 0);
            compare_exchange(local_data, local_size, partner, direction, rank);
            
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }

    // Сбор результатов
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
