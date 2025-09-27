#include <mpi.h>
#include <stdio.h>

double harmonic_series(int start, int end)
{
    double ret = 0;

    for (int i = start; i <= end; ++i)
        ret += 1.0/i;
    
    return ret;
}

int main(int argc, char *argv[])
{
    int comm_size = 0, my_rank = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int N = 0;
    if (my_rank == 0)
    {
        printf("Enter number of terms of harmonic series you want to compute: ");
        fflush(stdout);
        scanf("%d", &N);
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int number_of_terms_per_worker = N / comm_size;
    int start = my_rank * number_of_terms_per_worker + 1;
    int end = (my_rank + 1) * number_of_terms_per_worker;

    if (my_rank == comm_size - 1)
        end = N;
    
    double worker_sum = harmonic_series(start, end);

    double final_sum = 0;
    MPI_Reduce(&worker_sum, &final_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("Sum of first %d terms of harmonic series: %f\n", N, final_sum);
    }

    MPI_Finalize();
    return 0;
}
