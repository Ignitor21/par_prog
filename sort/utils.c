#include <stdio.h>
#include <stdlib.h>

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
    return;
}

void print_array(const int* arr, int size)
{
    for (int i = 0; i < size; ++i)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int getMax(int arr[], int n) {
    int max = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

// Сортировка подсчётом для определённого разряда (exp)
void countingSort(int arr[], int n, int exp) {
    int *output = (int *)malloc(n * sizeof(int));
    int count[10] = {0}; // 10 возможных цифр (0-9)

    // Подсчёт количества вхождений каждой цифры
    for (int i = 0; i < n; i++) {
        int digit = (arr[i] / exp) % 10;
        count[digit]++;
    }

    // Накопление суммы (чтобы получить правильные индексы)
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    // Построение выходного массива
    for (int i = n - 1; i >= 0; i--) {
        int digit = (arr[i] / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }

    // Копирование отсортированного массива обратно в arr
    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }

    free(output);
}

// Основная функция Radix Sort
void radixSort(int arr[], int n) {
    if (n <= 0) return;

    int max_num = getMax(arr, n);

    // Применяем Counting Sort для каждого разряда
    for (int exp = 1; max_num / exp > 0; exp *= 10) {
        countingSort(arr, n, exp);
    }
}
