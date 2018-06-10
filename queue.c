#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
 
/* 
 * Struct para representar uma Fila
 * Retira do início e adiciona no final.
 * Tamanho definido por [capacity].
 */
struct Queue {
    int front, back, size;
    unsigned capacity;
    int* array;
};
 
/* 
 * Cria struct Queue.
 */ 
struct Queue* createQueue(unsigned capacity) {
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0; 
    queue->back = capacity - 1;
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}
 
/* 
 * Retorna se fila está cheia.
 */ 
int isFull(struct Queue* queue) {  
    return (queue->size == queue->capacity);  
}
 
/* 
 * Retorna se fila está vazia.
 */  
int isEmpty(struct Queue* queue) {  
    return (queue->size == 0); 
}

/* 
 * Adiciona elemento no final da fila (se não estiver cheia).
 */ 
void push(struct Queue* queue, int item) {
    if (isFull(queue))
        return;
    queue->back = (queue->back + 1)%queue->capacity;
    queue->array[queue->back] = item;
    queue->size = queue->size + 1;
}

/* 
 * Retira último elemento da fila e o retorna.
 */  
int pop(struct Queue* queue) {
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

/* 
 * Retorna elemento do início da fila.
 */ 
int front(struct Queue* queue) {
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

/* 
 * Retorna elemento do fim da fila.
 */  
int back(struct Queue* queue) {
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->back];
}
