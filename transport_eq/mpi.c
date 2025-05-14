#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>

#include "utils.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, numprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);


    const double x_left = 0.0;
    const double x_right = 10.0;
    const double t_start = 0;
    const double t_end = 3;
    const double a = 2;
    const double b = 4;

    const double dx = 0.0001;
    const double dt = dx / 2;

    std::vector<double> x_vec;
    for (double x = x_left; x < x_right; x += dx)
        x_vec.push_back(x);

    const int N = x_vec.size();

    int local_n = N / numprocs;
    int remainder = N % numprocs;
    if (rank < remainder) {
        local_n += 1;
    }

    std::vector<double> u_current(local_n + 2, 0.0); 
    std::vector<double> u_next(local_n + 2, 0.0);

    int offset = 0;
    for (int r = 0; r < rank; ++r) {
        offset += (r < remainder) ? (N / numprocs + 1) : (N / numprocs);
    }

    for (int i = 1; i <= local_n; ++i) {
        double x = x_left + (offset + i - 1) * dx;
        u_current[i] = rectangle(x, a, b);
    }

    double t_cur = t_start;
    double start_time = MPI_Wtime();
    while (t_cur < t_end) {
        // Обмен ghost cells
        if (numprocs > 1) {
            // Обмен с левым соседом
            if (rank > 0) {
                MPI_Sendrecv(&u_current[1], 1, MPI_DOUBLE, rank-1, 0,
                             &u_current[0], 1, MPI_DOUBLE, rank-1, 0,
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            // Обмен с правым соседом
            if (rank < numprocs - 1) {
                MPI_Sendrecv(&u_current[local_n], 1, MPI_DOUBLE, rank+1, 0,
                             &u_current[local_n+1], 1, MPI_DOUBLE, rank+1, 0,
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        for (int i = 1; i <= local_n; ++i) {
            double term1 = (u_current[i+1] - u_current[i-1]) * ((dt)/(2*dx));
            double term2 = (u_current[i+1] - 2*u_current[i] + u_current[i-1]) * 0.5 * ((dt * dt) / (dx * dx));
            u_next[i] = u_current[i] - term1 + term2;
        }

        for (int i = 0; i <= local_n + 1; ++i) {
            u_current[i] = u_next[i];
        }
        t_cur += dt;
    }

    std::vector<double> global_u(N);
    double end_time;
    if (rank == 0) {
        for (int i = 1; i <= local_n; ++i) {
            global_u[offset + i - 1] = u_current[i];
        }
        for (int r = 1; r < numprocs; ++r) {
            int r_local_n = (r < remainder) ? (N / numprocs + 1) : (N / numprocs);
            std::vector<double> r_data(r_local_n);
            MPI_Recv(r_data.data(), r_local_n, MPI_DOUBLE, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int r_offset = 0;
            for (int prev_r = 0; prev_r < r; ++prev_r) {
                r_offset += (prev_r < remainder) ? (N / numprocs + 1) : (N / numprocs);
            }
            for (int i = 0; i < r_local_n; ++i) {
                global_u[r_offset + i] = r_data[i];
            }
        }
        
        end_time = MPI_Wtime();
    } else {
        std::vector<double> send_data(local_n);
        for (int i = 1; i <= local_n; ++i) {
            send_data[i-1] = u_current[i];
        }
        MPI_Send(send_data.data(), local_n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }


    if (rank == 0) 
    {
        double total_time = end_time - start_time;
        std::cout.precision(6);
        std::cout << "Время выполнения: " << std::fixed << total_time << " с" << std::endl;
        write_vec_to_file("u.txt", global_u);
        write_vec_to_file("x.txt", x_vec);
    }

    MPI_Finalize();
    return 0;
}
