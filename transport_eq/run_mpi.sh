mpic++ -O2 mpi.c utils.c -o mpi
mpirun -np 6 ./mpi
./plot.py
