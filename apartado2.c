#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "queue2.c"
#define LIBRE 0
#define OCUPADA 1

const int MAX_SLEEP = 4;

void master(int nprocs)
{
    int nClientes = 20;
    Cliente clientes[nClientes];
    Cola colaClientes;
    Cola colaDormidos;
    inicializarCola(&colaClientes);
    inicializarCola(&colaDormidos);
    time_t currentTime;
    int tiempoCliente;

    int estadoCajas[nprocs -1];
    for (int i = 0; i< nprocs-1;i++)
    {
        estadoCajas[i] = LIBRE;
    }
    
    for (int i = 0; i< nClientes; i++)
    {
        clientes[i].idCliente = i;
        clientes[i].isSleep = 0;
        clientes[i].sleepTimer = MAX_SLEEP;

        // Asignar prioritario o no prioritario
        if (i%2 == 0)
        {
            clientes[i].tipo = PRIORITARIO;
        }
        else
        {
            clientes[i].tipo = NO_PRIORITARIO;
        }
        meterUltimo(&colaClientes, clientes[i]);
    }
    
    int nClientesEnCola = longitudCola(&colaClientes); 
    int nClientesDormidos = 0;
    printf("Longitud de la cola: %d\n", nClientesEnCola);
    //mostrarCola(&colaClientes);
    
    int nCajasAbiertas = nprocs/2;
    int nClientesAtendidos = 0;
    int stop = -1;
    int maximosClientesAtendidos = 10;
    //int nClientesEnCola = nClientes;

    while ((nClientesEnCola > 0 || nClientesAtendidos < nClientes))
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
            if(estadoCajas[i] == LIBRE){
                
                Cliente clienteSacado;
                clienteSacado = sacarPrimero(&colaClientes);
                int idCliente = clienteSacado.idCliente;
                int tipoCliente = clienteSacado.tipo;

                if (tipoCliente == NO_PRIORITARIO)
                {
                    tiempoCliente = 5 + ((rand()) % 6);
                }
                else
                {
                    tiempoCliente = 10 + (rand() % 11);
                }

                MPI_Send(&idCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&tipoCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&tiempoCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                estadoCajas[i] = OCUPADA;
                nClientesEnCola--;
                //printf("longitud de la cola %d\n", longitudCola(&colaClientes));
            }
        }

        //verificar clientes dormidos y actualizar el contador
        //si un cliente tiene su tiempo a 0, se despierta y vuelve a la cola en última posición
        for(int i = 0; i < nClientesDormidos; i++)
        {
            
            if(colaDormidos.elementos[i].sleepTimer > 0)
            {
                colaDormidos.elementos[i].sleepTimer--;
            }
            else
            {
                if(nClientesAtendidos < maximosClientesAtendidos)
                {
                    Cliente temp;
                    temp = sacarPrimero(&colaDormidos);
                    temp.isSleep = 0;
                    temp.sleepTimer = MAX_SLEEP;
                    nClientesDormidos--;
                    meterUltimo(&colaClientes, temp);
                    nClientesEnCola++;
                    printf("El cliente %d ha vuelto a la cola, hay %d clientes en la cola\n", temp.idCliente, nClientesEnCola);
                }
            }

            
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
        int tipoClienteAtendido;

        MPI_Recv(&idClienteAtendido, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&idCaja, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&tipoClienteAtendido, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        estadoCajas[idCaja] = LIBRE;

        printf("Cliente %d, de tipo: %d, atendido en caja %d\n", idClienteAtendido,tipoClienteAtendido, idCaja);

        Cliente temp = colaClientes.elementos[idClienteAtendido];
        temp.isSleep = 1;
        temp.sleepTimer = MAX_SLEEP;
        temp.tipo = ((rand() % 10) < 7) ? 0 : 1;

        meterUltimo(&colaDormidos, temp);
        //mostrarCola(&colaDormidos);
        //mostrarCola(&colaClientes);

        nClientesDormidos++;
        nClientesAtendidos++;

        printf("numero de clientes atendidos: %d \n", nClientesAtendidos);
        printf("numero de clientes en la cola: %d \n", nClientesEnCola);
        //nClientesEnCola = longitudCola(&colaClientes); 

        if(nClientesAtendidos >= maximosClientesAtendidos)
        {
            printf("Se ha alcanzado el máximo de clientes atendidos. La cola ya no admitirá nuevos clientes\n");
        }

        sleep(1);
    }
    
    for(int i = 0; i < nprocs; i++)
    {
        MPI_Send(&stop, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    printf("Programa finalizado \n");
    
}

void slave(int rank)
{
    while(1)
    {
        srand(time(NULL));
        int idCliente;
        int tiempoCliente;
        int tipoCliente;

        MPI_Recv(&idCliente, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        if (idCliente == -1)
        {
            break;
        }
        MPI_Recv(&tipoCliente, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if(tipoCliente == NO_PRIORITARIO)
        {
            MPI_Recv(&tiempoCliente, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Yo, el cliente NORMAL %d, dormire: %d y estoy en la caja: %d\n", idCliente, tiempoCliente, rank);
            sleep(tiempoCliente);
        }
        else
        {
            MPI_Recv(&tiempoCliente, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Yo, el cliente VIP %d, dormire: %d y estoy en la caja: %d\n", idCliente, tiempoCliente, rank);
            sleep(tiempoCliente);
        }
        
        MPI_Send(&idCliente, 1, MPI_INT, 0,0, MPI_COMM_WORLD);
        MPI_Send(&rank, 1, MPI_INT, 0,0, MPI_COMM_WORLD);
        MPI_Send(&tipoCliente, 1, MPI_INT, 0,0, MPI_COMM_WORLD);

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


