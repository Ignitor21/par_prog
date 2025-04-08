#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

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

int main() {
    int n = 0;
    int *arr = read_array_from_file("input.txt", &n);
    printf("Размер массива: %d\n", n); 
    is_sorted(arr, n);

    clock_t start = clock();  // Засекаем начало
    bitonicSort(arr, n, true);
    clock_t end = clock();  // Засекаем конец

    is_sorted(arr, n);
    
    double time_spent = 1000 * (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("Время выполнения: %.0f мс\n", time_spent);
    free(arr);
    return 0;
}
