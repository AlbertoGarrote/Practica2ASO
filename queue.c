#include <stdio.h>
#include <stdlib.h>

// Definimos una constante para el tamaño máximo de la cola
#define MAX 20

// Estructura de la cola

   struct Cliente
    {
        int idCliente;
        int isSleep;
        int sleepTimer;
    };

struct Cola {
    struct Cliente elementos[MAX];
    int frente, final;
};

// Inicializar la cola
void inicializarCola(struct Cola* cola) {
    cola->frente = -1;
    cola->final = -1;
}

// Verifica si la cola está vacía
int estaVacia(struct Cola* cola) {
    return cola->frente == -1;
}

// Verifica si la cola está llena
int estaLlena(struct Cola* cola) {
    return cola->final == MAX - 1;
}

// Enfilear (agregar un elemento a la cola)
void enfilear(struct Cola* cola, struct Cliente valor) {
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

// Desenfilear (eliminar un elemento de la cola)
struct Cliente desenfilear(struct Cola* cola) {
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
        //printf("La cola está vacía.\n");
    } else {
        //printf("Elementos de la cola: ");
        for (int i = cola->frente; i <= cola->final; i++) {
            //printf("%d ", cola->elementos[i].idCliente);
        }
        //printf("\n");
    }
}

int longitudCola(struct Cola* cola)
{
    return cola->final;
}

