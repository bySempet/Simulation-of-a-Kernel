#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include "pcb.h"

// Estructura para un nodo de la cola de procesos (PCB)
typedef struct ProcesoNode {
    struct PCB* proceso;
    struct ProcesoNode* siguiente;
}ProcesoNode;

// Estructura para la cola de procesos
typedef struct ProcesoQueue {
    struct ProcesoNode* inicio;
    struct ProcesoNode* final;
    int prioridad;
    int cantidad;
    int maxCantidad;
}ProcesoQueue;

// Estructura para la cola de colas de procesos (Queue of Queues)
typedef struct ColaDeColas {
    struct ProcesoQueue* colas;
    int numColas;
}ColaDeColas;

// Inicializa una cola de procesos vacía con una prioridad dada
ProcesoQueue* inicializarProcesoQueue(int prioridad) {
    struct ProcesoQueue* queue = malloc(sizeof(struct ProcesoQueue));
    if (queue == NULL) 
    {
        printf("Error: No se pudo asignar memoria para la cola de procesos.\n");
        exit(1);
    }
    queue->inicio = NULL;
    queue->final = NULL;
    queue->prioridad = prioridad;
    queue->cantidad = 0;
    queue->maxCantidad = 50;
    return queue;
}

// Inicializa una cola de colas de procesos
 ColaDeColas* inicializarColaDeColas(int numColas) {
    struct ColaDeColas* colaDeColas = malloc(sizeof(struct ColaDeColas));
    if (colaDeColas == NULL) {
        printf("Error: No se pudo asignar memoria para la cola de colas.\n");
        exit(1);
    }
    colaDeColas->colas = malloc(numColas * sizeof(struct ProcesoQueue*));
    if (colaDeColas->colas == NULL) {
        printf("Error: No se pudo asignar memoria para las colas de procesos.\n");
        exit(1);
    }
    colaDeColas->numColas = numColas;
    for (int i = 0; i < numColas; i++) {
        colaDeColas->colas[i] = *inicializarProcesoQueue(i);
    }
    return colaDeColas;
}

// Agrega un proceso (PCB) al final de la cola de procesos
void enqueue(ProcesoQueue* queue,  PCB* proceso) {
    ProcesoNode* nuevoNodo = malloc(sizeof(struct ProcesoNode));
    if (nuevoNodo == NULL) 
    {
        printf("Error: No se pudo asignar memoria para el nodo del proceso.\n");
        exit(1);
    }
    nuevoNodo->proceso = proceso;
    nuevoNodo->siguiente = NULL;

    if (queue->inicio == NULL) 
    {
        queue->inicio = nuevoNodo;
        queue->final = nuevoNodo;
    } else 
    {
        queue->final->siguiente = nuevoNodo;
        queue->final = nuevoNodo;
    }
    queue->cantidad++;
}

void agregarAQueueDePrioridad(ColaDeColas* colaDeColas,   PCB* proceso) {
    if (proceso->prioridad < 0 || proceso->prioridad >= colaDeColas->numColas) 
    {
        printf("Error: Prioridad fuera de rango.\n");
        return;
    }
    if(colaDeColas->colas[proceso->prioridad].cantidad < colaDeColas->colas[proceso->prioridad].maxCantidad)
    enqueue(&colaDeColas->colas[proceso->prioridad], proceso);
    else
    {
        printf("Error: La cola de prioridad %d esta llena.\n",proceso->prioridad);
        return;
    }
}
// Verifica si la cola de procesos está vacía
int vacia(ProcesoQueue* queue) {
    if(queue->inicio == NULL) return 1;
   
    return 0;
    
}
// Elimina y devuelve el proceso (PCB) al inicio de la cola de procesos
 PCB* dequeue(ProcesoQueue* queue) {

    ProcesoNode* nodoAEliminar = queue->inicio;
    PCB* proceso = nodoAEliminar->proceso;
    
    queue->inicio = queue->inicio->siguiente;
    free(nodoAEliminar);

    if (queue->inicio == NULL) {
        queue->final = NULL;
    }
    queue->cantidad--;
    return proceso;
}

PCB * dequeueColas(ColaDeColas* colaDeColas) {
    for (int i = 0; i < colaDeColas->numColas; i++) {
        if (!vacia(&colaDeColas->colas[i])) {
            return dequeue(&colaDeColas->colas[i]);
        }
    }
    return NULL;
}



// Libera la memoria de la cola de procesos
void liberarProcesoQueue(ProcesoQueue* queue) {
    while (!vacia(queue)) {
        PCB* proceso = dequeue(queue);
        destroyPCB(proceso);
    }
    free(queue);
}

void liberarColaDeColas(ColaDeColas* colaDeColas) {
    for (int i = 0; i < colaDeColas->numColas; i++) {
        liberarProcesoQueue(&colaDeColas->colas[i]);
    }
    free(colaDeColas->colas);
    free(colaDeColas);
}

int obtenerPrioridadCola(ProcesoQueue* queue) {
    return queue->prioridad;
}