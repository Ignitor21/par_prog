#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int comm_size = 0, my_rank = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    
    int a = 21;

    if (my_rank == 0)
    {
        fprintf(stderr, "My rank: %d, initial value:  %d\n", my_rank, a);
        MPI_Send(&a, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }

    else 
    {
        MPI_Status status;
        MPI_Recv(&a, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, &status);
        MPI_Barrier(MPI_COMM_WORLD);
        fprintf(stderr, "My rank: %d, accepted value: %d\n", my_rank, a);
        MPI_Barrier(MPI_COMM_WORLD);
        ++a;
        int next_worker = (my_rank == comm_size - 1) ? 0 : my_rank + 1;
        MPI_Send(&a, 1, MPI_INT, next_worker, 0, MPI_COMM_WORLD);
    }

    if (my_rank == 0)
    {
        int final_value = 0;
        MPI_Status status;
        MPI_Recv(&a, 1, MPI_INT, comm_size - 1, 0, MPI_COMM_WORLD, &status);
        MPI_Barrier(MPI_COMM_WORLD);
        fprintf(stderr, "My rank: %d, final value:    %d\n", my_rank, a);
        MPI_Barrier(MPI_COMM_WORLD);
    } 

    MPI_Finalize();
    return 0;
}
