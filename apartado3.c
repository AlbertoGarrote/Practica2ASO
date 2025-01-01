#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "queue2.c"
#define LIBRE 0
#define OCUPADA 1

#define MAX_SLEEP 4;

void master(int nprocs)
{
    int nClientes = 20;
    Cliente clientes[nClientes];
    Cola colaClientes;
    Cola colaPrioritarios;
    Cola colaDormidos;
    inicializarCola(&colaPrioritarios);
    inicializarCola(&colaClientes);
    inicializarCola(&colaDormidos);
    time_t currentTime;
    int tiempoCliente;

    int estadoCajas[nprocs];
    estadoCajas[0] = OCUPADA;
    for (int i = 1; i< nprocs;i++)
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
            meterUltimo(&colaPrioritarios, clientes[i]);
        }
        else
        {
            clientes[i].tipo = NO_PRIORITARIO;
            meterUltimo(&colaClientes, clientes[i]);
        }
        
    }
    
    int nClientesEnCola = longitudCola(&colaClientes); 
    int nClientesEnColaPrioritaria = longitudCola(&colaPrioritarios);
    
    int nClientesDormidos = 0;
    printf("Longitud de la cola normal: %d\n", nClientesEnCola);
    printf("Longitud de la cola prioritaria: %d\n", nClientesEnColaPrioritaria);
    
    //mostrarCola(&colaClientes);
    
    int nCajas = nprocs-1;
   
    int cajasPrioritarias = nCajas/4;
    
    if(cajasPrioritarias < 1)
    {
        cajasPrioritarias = 1;
    }
    int cajasNormales = (nCajas-cajasPrioritarias);
    int nCajaNormalesAbiertas = cajasNormales/2;
    int nCajasPrioritariasAbiertas = cajasPrioritarias/2;
    int nCajasNormalesOcupadas = 0;
    int nCajasPrioritariasOcupadas = 0;

    int nClientesAtendidos = 0;
    int stop = -1;
    int maximosClientesAtendidos = 10;
    //int nClientesEnCola = nClientes;
    printf("%d Cajas normales: 1 - %d\n", cajasNormales, cajasNormales);
    printf("%d Cajas prioritarias: %d - %d\n", cajasPrioritarias, cajasNormales+1, cajasNormales+cajasPrioritarias);


    while ((nClientesEnCola+nClientesEnColaPrioritaria > 0 || nClientesAtendidos < nClientes))
    {
        //abrir cajas normales
        if(nClientesEnCola > 2 * nCajaNormalesAbiertas && nCajaNormalesAbiertas < cajasNormales)
        {
            nCajaNormalesAbiertas++;
            printf("caja normal abierta. %d cajas normales abiertas\n", nCajaNormalesAbiertas);
           
        }

        //abrir cajas prioritarias
        if(nClientesEnColaPrioritaria > 2 * nCajasPrioritariasAbiertas && nCajasPrioritariasAbiertas < cajasPrioritarias)
        {
            nCajasPrioritariasAbiertas++;
            printf("caja prioritaria abierta. %d cajas prioritarias abiertas\n", nCajasPrioritariasAbiertas);
            
        }

        //cerrar cajas normales
        if(nCajaNormalesAbiertas > nClientesEnCola && nCajaNormalesAbiertas > 0)
        {
            printf("caja normal cerrada\n");
            nCajaNormalesAbiertas--;
        }

        //cerrar cajas prioritarias
        if(nCajasPrioritariasAbiertas > nClientesEnColaPrioritaria && nCajasPrioritariasAbiertas > 0)
        {
            printf("caja prioritaria cerrada\n");
            nCajasPrioritariasAbiertas--;
        }

        //asignar clientes a cajas normales
        for (int i = 1; i <= nCajaNormalesAbiertas && nClientesEnCola > 0; i++)
        {
            if(estadoCajas[i] == LIBRE){

                Cliente clienteSacado;
                if(nCajasPrioritariasOcupadas >= nCajasPrioritariasAbiertas && nClientesEnColaPrioritaria > 0)
                {
                    clienteSacado = sacarPrimero(&colaPrioritarios);
                    int idCliente = clienteSacado.idCliente;
                    int tipoCliente = PRIORITARIO;
                    tiempoCliente = 10 + (rand() % 11);
                    
                    MPI_Send(&idCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    MPI_Send(&tipoCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    MPI_Send(&tiempoCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    nCajasNormalesOcupadas++;
                    estadoCajas[i] = OCUPADA;
                    nClientesEnColaPrioritaria--;
                }else
                {
                    clienteSacado = sacarPrimero(&colaClientes);
                    int idCliente = clienteSacado.idCliente;
                    int tipoCliente = NO_PRIORITARIO;
                    tiempoCliente = 5 + ((rand()) % 6);
                    MPI_Send(&idCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    MPI_Send(&tipoCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    MPI_Send(&tiempoCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    nCajasNormalesOcupadas++;
                    estadoCajas[i] = OCUPADA;
                    nClientesEnCola--;
                }
                
                
            }
        }

        //asignar clientes a cajas prioritarios
        for (int i = cajasNormales+1; i <= cajasNormales+nCajasPrioritariasAbiertas && nClientesEnColaPrioritaria > 0; i++)
        {
            
            if(estadoCajas[i] == LIBRE){
                
                Cliente clienteSacado;
                clienteSacado = sacarPrimero(&colaPrioritarios);
                int idCliente = clienteSacado.idCliente;
                int tipoCliente = PRIORITARIO;
                tiempoCliente = 10 + (rand() % 11);
                /*
                if (tipoCliente == NO_PRIORITARIO)
                {
                    tiempoCliente = 5 + ((rand()) % 6);
                }
                else
                {
                    tiempoCliente = 10 + (rand() % 11);
                }
                */
                MPI_Send(&idCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&tipoCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&tiempoCliente, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                nCajasPrioritariasOcupadas++;
                estadoCajas[i] = OCUPADA;
                nClientesEnColaPrioritaria--;
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
                    int tipoCliente = temp.tipo;
                    nClientesDormidos--;
                    if(tipoCliente == PRIORITARIO)
                    {
                        meterUltimo(&colaPrioritarios, temp);
                        nClientesEnColaPrioritaria++;
                        printf("El cliente %d ha vuelto a la cola, hay %d clientes en la cola\n", temp.idCliente, nClientesEnCola);
                    }else{
                        meterUltimo(&colaClientes, temp);
                        nClientesEnCola++;
                        printf("El cliente %d ha vuelto a la cola, hay %d clientes en la cola\n", temp.idCliente, nClientesEnColaPrioritaria);
                    }
                    
                    
                    
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
        if(idCaja <= cajasNormales)
        {
            nCajasNormalesOcupadas--;
        }
        else
        {
            nCajasPrioritariasOcupadas--;
        }

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
        printf("numero de clientes en la cola normal: %d \n", nClientesEnCola);
        printf("numero de clientes en la cola prioritaria: %d \n", nClientesEnColaPrioritaria);
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


