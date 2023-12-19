#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// Estructura para representar el tiempo de vida de un proceso
typedef struct TiempoDeVida {
    time_t creacion; // Momento en que se creó el proceso
    int tiempo_asignado; // Tiempo asignado hasta que el proceso muere
};

// Estructura PCB con la nueva variable de estado del proceso
typedef struct PCB {
    int identificador; // Identificador unequivoco de cada proceso
    int prioridad;  // Prioridad asignada al proceso
    struct TiempoDeVida tiempo_de_vida;
    int estado; // Estado del proceso(running 2, bloqueado 1, waiting 0)
} PCB;


// Función para inicializar un PCB
 PCB* inicializarPCB(int id, int prioridad, int estado, int tiempo_asignado) {
    PCB* pcb = (PCB*)malloc(sizeof(struct PCB));
    if (pcb == NULL) {
        printf("Error: No se pudo asignar memoria para el PCB.\n");
        exit(1);
    }

    pcb->identificador = id;
    pcb->prioridad = prioridad;
    
    // Obtener el momento de creación
    pcb->tiempo_de_vida.creacion = time(NULL);
    
    pcb->tiempo_de_vida.tiempo_asignado = tiempo_asignado;
    
    // Asignar el estado
    pcb->estado = estado;

    return pcb;
}

#include "pcb.h"

// Función para obtener el identificador de un PCB
int obtenerIdentificador(struct PCB* pcb) {
    return pcb->identificador;
}

// Función para establecer el identificador de un PCB
void establecerIdentificador(struct PCB* pcb, int id) {
    pcb->identificador = id;
}

// Función para obtener la prioridad de un PCB
int obtenerPrioridad(struct PCB* pcb) {
    return pcb->prioridad;
}

// Función para establecer la prioridad de un PCB
void establecerPrioridad(struct PCB* pcb, int prioridad) {
    pcb->prioridad = prioridad;
}

// Función para obtener el estado de un PCB
int obtenerEstado(struct PCB* pcb) {
    return pcb->estado;
}

// Función para establecer el estado de un PCB
void establecerEstado(struct PCB* pcb, int estado) {
    pcb->estado = estado;
}

// Función para obtener el tiempo de creación de un PCB
time_t obtenerMomentoCreacion(struct PCB* pcb) {
    return pcb->tiempo_de_vida.creacion;
}

// Función para obtener el tiempo asignado de un PCB
int obtenerTiempoAsignado(struct PCB* pcb) {
    return pcb->tiempo_de_vida.tiempo_asignado;
}

// Función para establecer el tiempo asignado de un PCB
void establecerTiempoAsignado(struct PCB* pcb, int tiempo_asignado) {
    pcb->tiempo_de_vida.tiempo_asignado = tiempo_asignado;
}

// Función para obtener el estado de un PCB en forma legible
const char* obtenerEstadoLegible(struct PCB* pcb) {
    if (pcb->estado == 2) {
        return "Running";
    } else if (pcb->estado == 1) {
        return "Blocked";
    } else if (pcb->estado == 0) {
        return "Waiting";
    } else {
        return "Desconocido";
    }
}

// Función para cambiar el estado de un PCB
void cambiarEstado(struct PCB* pcb, int nuevoEstado) {
    pcb->estado = nuevoEstado;
}



