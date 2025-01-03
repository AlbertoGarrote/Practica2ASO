//C program to Demonstrate Priority Queue
#include<stdio.h>
#include<limits.h>
#include"queue2.c"

#define MAX 100








typedef struct {
    // denotes where the last item in priority queue is
    // initialized to -1 since no item is in queue
    int idx;
    // pqVal holds data for each index item
    // pqPriority holds priority for each index item
    Cliente pqVal[MAX];
    int pqPriority[MAX];
} ColaPrioridad;

void inicializarColaPrioridad(ColaPrioridad *q) {
    q->idx = -1;
}


int isEmptyP (ColaPrioridad *q)
{
  return q->idx == -1;
}

int
isFullP (ColaPrioridad *q)
{
  return q->idx == MAX - 1;
}

// enqueue just adds item to the end of the priority queue | O(1)
void enqueue (ColaPrioridad *q, Cliente data, int priority)
{
  if (!isFullP (q))
    {

      // Increase the index
      q->idx++;

      // Insert the element in priority queue
      q->pqVal[q->idx] = data;
      q->pqPriority[q->idx] = priority;
    }
}

// returns item with highest priority
// NOTE: Max Priority Queue High priority number means higher priority | O(N)
int peek (ColaPrioridad *q)
{
  // Note : Max Priority, so assigned min value as initial value
  int maxPriority = INT_MIN;
  int indexPos = -1;

  // Linear search for highest priority
  for (int i = 0; i <= q->idx; i++)
    {
      // If two items have same priority choose the one with 
      // higher data value 
      if (maxPriority == q->pqPriority[i] && indexPos > -1
	  && q->pqVal[indexPos].idCliente > q->pqVal[i].idCliente)
	{
	  maxPriority = q->pqPriority[i];
	  indexPos = i;
	}

      // note: using MAX Priority so higher priority number
      // means higher priority
      else if (maxPriority < q->pqPriority[i])
	{
	  maxPriority = q->pqPriority[i];
	  indexPos = i;
	}
    }

  // Return index of the element where 
  return indexPos;
}

// This removes the element with highest priority
// from the priority queue | O(N)
Cliente dequeue (ColaPrioridad *q)
{
  if (!isEmptyP (q))
    {
      // Get element with highest priority
      int indexPos = peek (q);
      Cliente cliente = q->pqVal[indexPos];
      // reduce size of priority queue by first
      // shifting all elements one position left
      // from index where the highest priority item was found
      for (int i = indexPos; i < q->idx; i++)
	{
	  q->pqVal[i] = q->pqVal[i + 1];
	  q->pqPriority[i] = q->pqPriority[i + 1];
	}

      // reduce size of priority queue by 1
      q->idx--;
      return cliente;
    }
}
