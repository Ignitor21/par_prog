#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Функция для слияния двух подмассивов
void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Создаем временные подмассивы
    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    // Копируем данные во временные массивы
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    // Слияние временных массивов обратно в arr[left..right]
    int i = 0, j = 0, k = left;
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

    // Копируем оставшиеся элементы L[], если они есть
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Копируем оставшиеся элементы R[], если они есть
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

void verify_solution(int* arr, int size)
{
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            printf("Массив не отсортирован\n");
            return;
        }
    }
    printf("Массив отсортирован\n");  // Найдены два элемента в неправильном порядке
    return;
}

// Рекурсивная функция сортировки слиянием
void mergeSort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}

// Функция для чтения массива из файла
int readArrayFromFile(const char *filename, int **arr) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Ошибка открытия файла");
        return -1;
    }

    int count = 0;
    int num;
    while (fscanf(file, "%d", &num) == 1) {
        count++;
    }

    *arr = (int *)malloc(count * sizeof(int));
    if (*arr == NULL) {
        perror("Ошибка выделения памяти");
        fclose(file);
        return -1;
    }

    rewind(file);
    for (int i = 0; i < count; i++) {
        fscanf(file, "%d", &(*arr)[i]);
    }

    fclose(file);
    return count;
}

int main() {
    int *arr = NULL;
    int size;

    // Чтение массива из файла
    size = readArrayFromFile("input.txt", &arr);
    if (size <= 0) {
        return 1;
    }

    // Замер времени начала сортировки
    clock_t start = clock();

    // Сортировка слиянием
    mergeSort(arr, 0, size - 1);

    // Замер времени окончания сортировки
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    // Освобождение памяти
    verify_solution(arr, size); 
    free(arr);
    // Вывод времени работы
    printf("Время выполнения сортировки: %.6f секунд\n", time_spent);

    return 0;
}
