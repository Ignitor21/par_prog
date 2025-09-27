#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{

        int a = 21;
#pragma omp parallel shared(a)
    {
        int num_threads = omp_get_num_threads();
        int tid = omp_get_thread_num();

        if (tid == 0) 
            printf("My tid: %d, initial value: %d\n", tid, a++);

        for (int i = 1; i < num_threads; ++i)
        {
            if (tid == i)
            {
                printf("My tid: %d, accepted value: %d\n", tid, a++);
            }
            #pragma omp barrier // Ждем, чтобы следующий поток начал после завершения текущего
        }
        if (tid == 0)
            printf("My tid: %d, final value: %d\n", tid, a);
    }
     
    return 0;
}

