#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

double harmonic_series(int start, int end)
{
    double ret = 0;

    for (int i = start; i <= end; ++i)
        ret += 1.0/i;
    
    return ret;
}

int main(int argc, char *argv[])
{
    int N = 0;
    if (argc != 2)
    {
        printf("Usage: ./a.out N, where N - number of termes of harmonic series you want to compute\n");
        return 1;
    }
    else
    {
       N = atoi(argv[1]);
       printf("%d\n", N);
    }

    double final_sum = 0;
    #pragma omp parallel
    {
        int thread_number = omp_get_num_threads();
        int my_number = omp_get_thread_num();

        int number_of_terms_per_thread = N / thread_number;
        int start = my_number * number_of_terms_per_thread + 1;
        int end = (my_number + 1) * number_of_terms_per_thread;

        if (my_number == thread_number  - 1)
            end = N;
        
        double thread_sum = harmonic_series(start, end);
        #pragma omp critical
        {
            final_sum += thread_sum;
        }
    }

    printf("Sum of first %d terms of harmonic series: %f\n", N, final_sum);
    return 0;
}
