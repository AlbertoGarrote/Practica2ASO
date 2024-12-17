#include <stdio.h>
#include "mpi.h"
int main(int argc, char **argv)
{
 int rank, nprocs;
 MPI_Init(&argc, &argv);
 MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 printf("Hola mundo, soy el proceso %d de los %d que se están ejecutando\n", rank, nprocs);
 MPI_Finalize();
 return 0;
}


