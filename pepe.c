#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "queue.c"


void master(int nprocs)
{
    int nClientes = 20;
    struct Cliente clientes[nClientes];
    struct Cola colaClientes;
    inicializarCola(&colaClientes);
    
    for (int i = 0; i< nClientes; i++)
    {
        clientes[i].idCliente = i;
        clientes[i].isSleep = 0;
        clientes[i].sleepTimer = 5;
        enfilear(&colaClientes, clientes[i]);
    }
    
    int nClientesEnCola = longitudCola(&colaClientes); 
    printf("Longitud de la cola: %d\n", nClientesEnCola);
    //mostrarCola(&colaClientes);
    
    int nCajasAbiertas = nprocs/2;
    int nClientesAtendidos = 0;
    int stop = -1;
    //int nClientesEnCola = nClientes;

    while (nClientesEnCola > 0 || nClientesAtendidos < nClientes)
    {
        //abrir cajas
        if(nClientesEnCola > 2 * nCajasAbiertas && nCajasAbiertas < nprocs -1)
        {
            nCajasAbiertas++;
        }

        //cerrar cajas
        if(nCajasAbiertas > nClientesEnCola && nCajasAbiertas > 0)
        {
            nCajasAbiertas--;
        }

        //asignar clientes
        for (int i = 1; i <= nCajasAbiertas && nClientesEnCola > 0; i++)
        {
            int tiempoCliente = 5 + ((rand()) % 6);
            int idCliente = desenfilear(&colaClientes).idCliente;
            MPI_Send(&idCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&tiempoCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            nClientesEnCola--;
            //printf("longitud de la cola %d\n", longitudCola(&colaClientes));
        }

        
        // for(int i = 1; i<= nCajasAbiertas && nClientesAtendidos < 20; i++)
        // {
        //     int idClienteAtendido;
        //     MPI_Recv(&idClienteAtendido, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //     printf("Cliente %d atendido en caja %d\n", idClienteAtendido, i);

        //     nClientesAtendidos++;
        // }

        // Recibir mensaje de clientes atendidos
        int idClienteAtendido;
        int idCaja;
        MPI_Recv(&idClienteAtendido, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&idCaja, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Cliente %d atendido en caja %d\n", idClienteAtendido, idCaja);
        nClientesAtendidos++;

        printf("numero de clientes atendidos: %d \n", nClientesAtendidos);
        //nClientesEnCola = longitudCola(&colaClientes); 

        // Volver a meter clientes en cola de forma aleatoria
        // int auxiliarAleatoria = rand() % 10;
        // if (auxiliarAleatoria < 3)
        // {
        //     printf("Cliente %d vuelvo a la cola para ser atendido.\n", idClienteAtendido);
        //     enfilear(&colaClientes, clientes[idClienteAtendido]);
        //     nClientesEnCola = longitudCola(&colaClientes); 
        // }
        // printf("numero de clientes en la cola: %d \n", nClientesEnCola);

        sleep(1);
    }
    
    for(int i = 0; i < nprocs; i++)
    {
        MPI_Send(&stop, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    
}

void slave(int rank)
{
    while(1)
    {
        srand(time(NULL));
        int idCliente;
        int tiempoCliente;

        MPI_Recv(&idCliente, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        if (idCliente == -1)
        {
            break;
        }

        MPI_Recv(&tiempoCliente, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Yo, el cliente %d, dormire: %d y estoy en la caja: %d\n", idCliente, tiempoCliente, rank);
        
        sleep(tiempoCliente);

        MPI_Send(&idCliente, 1, MPI_INT, 0,0, MPI_COMM_WORLD);
        MPI_Send(&rank, 1, MPI_INT, 0,0, MPI_COMM_WORLD);

    }
}

int main(int argc, char **argv)
{
//printf("iniciando");
 int rank, nprocs;
 MPI_Init(&argc, &argv);
 MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 //printf("Hola mundo, soy el proceso %d de los %d que se están ejecutando\n", rank, nprocs);

 if (nprocs < 2)
    {
        if(rank == 0)
        {
            printf("Se requieren al menos 2 procesos");
        }
        MPI_Finalize();
        return 1;
    }
    if(rank == 0)
    {
        master(nprocs);

    }else
    {
        slave(rank);
    }
 MPI_Finalize();
 return 0;

}


