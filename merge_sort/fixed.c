#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void merge(int *arr, int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSort(int *arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

int readArrayFromFile(const char *filename, int **arr) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    int count = 0;
    int num;
    while (fscanf(file, "%d", &num) == 1) count++;

    *arr = (int *)malloc(count * sizeof(int));
    if (!*arr) {
        perror("Memory allocation failed");
        fclose(file);
        return -1;
    }

    rewind(file);
    for (int i = 0; i < count; i++)
        fscanf(file, "%d", &(*arr)[i]);

    fclose(file);
    return count;
}

void verify_solution(int* arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            printf("Массив не отсортирован\n");
            return;
        }
    }
    printf("Массив отсортирован\n");
}

void print_array(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void parallel_merge(int *arr, int num_chunks, int *chunk_sizes, int *displs) {
    // Создаем временный буфер для слияния
    int total_size = displs[num_chunks-1] + chunk_sizes[num_chunks-1];
    int *temp = (int *)malloc(total_size * sizeof(int));
    
    // Копируем исходные данные
    for (int i = 0; i < total_size; i++) {
        temp[i] = arr[i];
    }

    // Многофазное слияние
    int active_chunks = num_chunks;
    while (active_chunks > 1) {
        int new_chunks = 0;
        int new_displs[num_chunks];
        int new_sizes[num_chunks];
        
        for (int i = 0; i < active_chunks; i += 2) {
            if (i+1 >= active_chunks) {
                // Нечетное количество - копируем последний чанк
                new_displs[new_chunks] = displs[i];
                new_sizes[new_chunks] = chunk_sizes[i];
                new_chunks++;
                continue;
            }

            // Сливаем два соседних чанка
            int left_start = displs[i];
            int left_end = left_start + chunk_sizes[i];
            int right_start = displs[i+1];
            int right_end = right_start + chunk_sizes[i+1];
            
            int l = left_start, r = right_start, k = left_start;
            while (l < left_end && r < right_end) {
                if (temp[l] <= temp[r]) {
                    arr[k++] = temp[l++];
                } else {
                    arr[k++] = temp[r++];
                }
            }
            while (l < left_end) arr[k++] = temp[l++];
            while (r < right_end) arr[k++] = temp[r++];
            
            // Обновляем информацию о чанках
            new_displs[new_chunks] = displs[i];
            new_sizes[new_chunks] = chunk_sizes[i] + chunk_sizes[i+1];
            new_chunks++;
        }
        
        // Обновляем displs и chunk_sizes для следующей итерации
        for (int i = 0; i < new_chunks; i++) {
            displs[i] = new_displs[i];
            chunk_sizes[i] = new_sizes[i];
        }
        
        // Копируем обратно в temp для следующей итерации
        for (int i = 0; i < total_size; i++) {
            temp[i] = arr[i];
        }
        
        active_chunks = new_chunks;
    }
    
    free(temp);
}

int main(int argc, char **argv) {
    int rank, world_size;
    int *global_arr = NULL;
    int global_size;
    int *local_arr = NULL;
    int local_size;
    double start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (rank == 0) {
        global_size = readArrayFromFile("input.txt", &global_arr);
        if (global_size <= 0) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        start = MPI_Wtime();
    }

    MPI_Bcast(&global_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Распределяем данные
    int *sendcounts = (int *)malloc(world_size * sizeof(int));
    int *displs = (int *)malloc(world_size * sizeof(int));
    
    int base_size = global_size / world_size;
    int remainder = global_size % world_size;
    
    for (int i = 0; i < world_size; i++) {
        sendcounts[i] = base_size + (i < remainder ? 1 : 0);
        displs[i] = (i == 0) ? 0 : displs[i-1] + sendcounts[i-1];
    }

    local_size = sendcounts[rank];
    local_arr = (int *)malloc(local_size * sizeof(int));
    
    MPI_Scatterv(global_arr, sendcounts, displs, MPI_INT,
                local_arr, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Локальная сортировка
    mergeSort(local_arr, 0, local_size - 1);

    // Сбор результатов
    int *sorted_global = NULL;
    if (rank == 0) {
        sorted_global = (int *)malloc(global_size * sizeof(int));
    }

    MPI_Gatherv(local_arr, local_size, MPI_INT,
               sorted_global, sendcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    // Финальное слияние
    if (rank == 0) {
        parallel_merge(sorted_global, world_size, sendcounts, displs);
        
        end = MPI_Wtime();
        verify_solution(sorted_global, global_size);
        //print_array(sorted_global, global_size);
        printf("Время работы: %.6f секунд\n", end - start);
        
        free(sorted_global);
    }

    // Освобождение памяти
    free(local_arr);
    free(sendcounts);
    free(displs);
    if (rank == 0) {
        free(global_arr);
    }

    MPI_Finalize();
    return 0;
}
