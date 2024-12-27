#include <stdio.h>
#include <stdlib.h>

#define nElementos 20
#define NO_PRIORITARIO 0
#define PRIORITARIO 1

struct Cliente
{
    int idCliente;
    int isSleep;
    int sleepTimer;
    int tipo;
};

struct Cola {
    struct Cliente elementos[nElementos];
    int frente, final, idx;
};

// Constructor de la cola
void inicializarCola(struct Cola* cola) {
    cola->frente = -1;
    cola->final = -1;
    cola->idx = -1;
}

// Verifica si la cola está vacía
int estaVacia(struct Cola* cola) {
    return cola->frente == -1;
}

// Verifica si la cola está llena
int estaLlena(struct Cola* cola) {
    return cola->final == nElementos - 1;
}

// (agregar un elemento a la cola)
void meterUltimo(struct Cola* cola, struct Cliente valor) {
    if (estaLlena(cola)) {
        //printf("La cola está llena, no se puede enfilear.\n");
    } else {
        if (cola->frente == -1) {
            cola->frente = 0;  // Si la cola estaba vacía, asignamos el primer valor al frente
        }
        cola->final++;
        cola->elementos[cola->final] = valor;
        //printf("Enfileado %d\n", valor.idCliente);
    }
}

// (eliminar un elemento de la cola)
struct Cliente sacarPrimero(struct Cola* cola) {
    struct Cliente cliente;
    if (estaVacia(cola)) {
        //printf("La cola está vacía, no se puede desenfilear.\n");
        return cliente;
    } else {
        cliente = cola->elementos[cola->frente];
        if (cola->frente == cola->final) {
            cola->frente = cola->final = -1;  // La cola queda vacía
        } else {
            cola->frente++;
        }
        //printf("Desenfileado %d\n", cliente.idCliente);
        return cliente;
    }
}

// Función para mostrar el contenido de la cola
void mostrarCola(struct Cola* cola) {
    if (estaVacia(cola)) {
        printf("La cola está vacía.\n");
    } else {
        printf("Elementos de la cola: \n");
        for (int i = cola->frente; i <= cola->final; i++) {
            printf("%d,%d \n", cola->elementos[i].idCliente,cola->elementos[i].tipo);
        }
        //printf("\n");
    }
}

int longitudCola(struct Cola* cola)
{
    return cola->final+1;
}
