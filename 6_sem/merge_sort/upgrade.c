#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void merge(int arr[], int left, int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;
  int *L = (int *)malloc(n1 * sizeof(int));
  int *R = (int *)malloc(n2 * sizeof(int));

  for (int i = 0; i < n1; i++)
    L[i] = arr[left + i];
  for (int j = 0; j < n2; j++)
    R[j] = arr[mid + 1 + j];

  int i = 0, j = 0, k = left;
  while (i < n1 && j < n2) {
    if (L[i] <= R[j])
      arr[k++] = L[i++];
    else
      arr[k++] = R[j++];
  }
  while (i < n1)
    arr[k++] = L[i++];
  while (j < n2)
    arr[k++] = R[j++];

  free(L);
  free(R);
}

void insertionSort(int arr[], int left, int right) {
  for (int i = left + 1; i <= right; i++) {
    int key = arr[i];
    int j = i - 1;
    while (j >= left && arr[j] > key) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

void hybridMergeSort(int arr[], int left, int right) {
  const int min_merge_sort_size = 70;
  if (right - left + 1 <= min_merge_sort_size) {
    insertionSort(arr, left, right);
  } else {
    int mid = left + (right - left) / 2;
    hybridMergeSort(arr, left, mid);
    hybridMergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
  }
}

void verify_solution(int* arr, int size)
{
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            printf("Массив не отсортирован\n");
            return;
        }
    }
    printf("Массив отсортирован\n");
    return;
}

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

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Ошибка: не указано имя файла\n");
        printf("Использование: %s <имя_файла>\n", argv[0]);
        return 1;
    }

    char* filename = argv[1];
    int *arr = NULL;
    int size;

    size = readArrayFromFile(filename, &arr);
    if (size <= 0) {
        return 1;
    }

    clock_t start = clock();

    hybridMergeSort(arr, 0, size - 1);

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    verify_solution(arr, size); 
    printf("Время выполнения: %.6f с\n", time_spent);
    free(arr);

    return 0;
}
