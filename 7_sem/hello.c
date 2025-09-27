#include <omp.h>
#include <stdio.h>

int main (void)
{
    #pragma omp parallel
    printf ("Hellow, World! I am thread %d from %d\n", omp_get_thread_num(), omp_get_num_threads());
    return 0;
}
