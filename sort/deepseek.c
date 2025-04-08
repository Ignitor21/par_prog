#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

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

// Функция для обмена двух элементов
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Итеративная битоническая сортировка
void bitonicSort(int arr[], int n, bool ascending) {
    for (int k = 2; k <= n; k *= 2) {          // Размер битонической последовательности (2, 4, 8, ...)
        for (int j = k / 2; j > 0; j /= 2) {   // Расстояние между сравниваемыми элементами
            for (int i = 0; i < n; i++) {       // Проход по всему массиву
                int l = i ^ j;                 // Индекс элемента для сравнения
                
                // Сравниваем и обмениваем элементы в зависимости от порядка сортировки
                if (l > i) {
                    if (((i & k) == 0) == ascending) {
                        if (arr[i] > arr[l]) {
                            swap(&arr[i], &arr[l]);
                        }
                    } else {
                        if (arr[i] < arr[l]) {
                            swap(&arr[i], &arr[l]);
                        }
                    }
                }
            }
        }
    }
}

// Вспомогательная функция для вывода массива
void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int n = 0;
    int* arr = read_array_from_file("input.txt", &n);
    printf("Размер массива: %d\n", n); 
    is_sorted(arr, n);
    /* printf("Исходный массив:\n"); */
    /* printArray(arr, n); */
    
    // Сортируем массив в порядке возрастания
    bitonicSort(arr, n, true);
    
    is_sorted(arr, n);
    /* printArray(arr, n); */
     
    free(arr);
    return 0;
}
