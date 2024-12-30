#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define nElementos 20

typedef struct 
{
    int idCliente;
    int isSleep;
    int sleepTimer;
}Cliente;

typedef struct {
    Cliente elementos[nElementos];
    int front;
    int rear;
} Cola;

// Inicializa la cola
void inicializarCola(Cola *q) {
    q->front = -1;
    q->rear = -1;
}

// Verifica si la cola está vacía
int isEmpty(Cola *q) {
    return q->front == -1;
}

// Verifica si la cola está llena
int isFull(Cola *q) {
    return (q->rear + 1) % nElementos == q->front;
}

// Inserta un elemento en la cola
void meterUltimo(Cola *q, Cliente value) {
    if (isFull(q)) {
        printf("Error: La cola está llena.\n");
        return;
    }
    if (isEmpty(q)) {
        q->front = 0;
    }
    q->rear = (q->rear + 1) % nElementos;
    q->elementos[q->rear] = value;
    //printf("Se insertó el elemento {id: %d, name: %s} en la cola.\n", value.id, value.name);
}

// Elimina un elemento de la cola
Cliente sacarPrimero(Cola *q) {
    if (isEmpty(q)) {
        printf("Error: La cola está vacía.\n");
        Cliente emptyItem = {-1, -1, -1}; // Elemento vacío para devolver en caso de error
        return emptyItem;
    }
    Cliente value = q->elementos[q->front];
    if (q->front == q->rear) { // Si sólo hay un elemento
        q->front = -1;
        q->rear = -1;
    } else {
        q->front = (q->front + 1) % nElementos;
    }
    //printf("Se eliminó el elemento {id: %d, name: %s} de la cola.\n", value.id, value.name);
    return value;
}

int longitudCola(Cola *q) {
    if (isEmpty(q)) {
        return 0; // Si la cola está vacía, la longitud es 0
    }
    if (q->rear >= q->front) {
        return q->rear - q->front + 1; // Caso normal
    } else {
        return nElementos - q->front + q->rear + 1; // Caso circular
    }
}