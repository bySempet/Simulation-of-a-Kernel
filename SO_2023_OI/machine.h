#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include "process_queue.h"

// Estructura para representar un hilo
typedef struct {
    int id; // Identificador del hilo
    int ocupado; // 0 si el hilo está libre, 1 si está ocupado
    PCB *pcb; // Referencia al PCB asociado a este hilo
   
} Thread;

// Estructura para representar un núcleo
typedef struct {
    int id; // Identificador del núcleo
    int numThreads; // Número de hilos en el núcleo
    Thread *threads; // Arreglo de hilos en el núcleo
   
} Core;

// Estructura para representar una CPU
typedef struct {
    int id; // Identificador de la CPU
    int numCores; // Número de núcleos en la CPU
    Core *cores; // Arreglo de núcleos en la CPU
    
} CPU;

// Estructura para representar una máquina
typedef struct {
    int numCPUs; // Número de CPUs en la máquina
    CPU *cpus; // Arreglo de CPUs
  
} Machine;


// Función para crear un hilo con un PCB asociado
Thread *createThread(int id) {
    Thread *thread = malloc(sizeof(Thread));
    if (thread == NULL) {
        perror("Error al asignar memoria para hilo");
        exit(EXIT_FAILURE);
    }

    thread->pcb =(PCB *) malloc(sizeof(PCB));
    thread->id = id;
    thread->ocupado = 0;

    return thread;
}

// Función para crear un núcleo con hilos
Core *createCore(int id, int numThreads) {
    Core *core = malloc(sizeof(Core));
    if (core == NULL) {
        perror("Error al asignar memoria para núcleo");
        exit(EXIT_FAILURE);
    }
    core->id = id;
    core->numThreads = numThreads;
    core->threads = malloc(numThreads * sizeof(Thread));
    if (core->threads == NULL) {
        perror("Error al asignar memoria para hilos en el núcleo");
        exit(EXIT_FAILURE);
    }
    // Inicializar hilos en el núcleo
    for (int i = 0; i < numThreads; i++) {
        core->threads[i] = *createThread(i);
    }

    return core;
}

// Función para crear una CPU con núcleos
CPU *createCPU(int id, int numCores, int numThreadsPerCore) {
    CPU *cpu = malloc(sizeof(CPU));
    if (cpu == NULL) {
        perror("Error al asignar memoria para CPU");
        exit(EXIT_FAILURE);
    }
    cpu->id = id;
    cpu->numCores = numCores;
    cpu->cores = malloc(numCores * sizeof(Core));
    if (cpu->cores == NULL) {
        perror("Error al asignar memoria para núcleos en la CPU");
        exit(EXIT_FAILURE);
    }
    // Inicializar núcleos en la CPU
    for (int i = 0; i < numCores; i++) {
        cpu->cores[i] = *createCore(i, numThreadsPerCore);
    }

    return cpu;
}

// Función para crear una máquina con CPUs
Machine *createMachine(int numCPUs, int numCoresPerCPU, int numThreadsPerCore) {
    Machine *machine = malloc(sizeof(Machine));
    if (machine == NULL) {
        perror("Error al asignar memoria para la máquina");
        exit(EXIT_FAILURE);
    }
    machine->numCPUs = numCPUs;
    machine->cpus = malloc(numCPUs * sizeof(CPU));
    if (machine->cpus == NULL) {
        perror("Error al asignar memoria para CPUs en la máquina");
        exit(EXIT_FAILURE);
    }
    // Inicializar CPUs en la máquina
    for (int i = 0; i < numCPUs; i++) {
        machine->cpus[i] = *createCPU(i, numCoresPerCPU, numThreadsPerCore);
    }

    return machine;
}


// Función para destruir un hilo
void destroyThread(Thread *thread) {
    destroyPCB(thread->pcb);
    free(thread);
}

// Función para destruir un núcleo
void destroyCore(Core *core) {
    for (int i = 0; i < core->numThreads; i++) {
        destroyThread(&(core->threads[i]));
    }
    free(core->threads);
    free(core);
}

// Función para destruir una CPU
void destroyCPU(CPU *cpu) {
    for (int i = 0; i < cpu->numCores; i++) {
        destroyCore(&(cpu->cores[i]));
    }
    free(cpu->cores);
    free(cpu);
}

// Función para destruir una máquina
void destroyMachine(Machine *machine) {
    for (int i = 0; i < machine->numCPUs; i++) {
        destroyCPU(&(machine->cpus[i]));
    }
    free(machine->cpus);
    free(machine);
}

void restarTiempo(Machine * Machine, int tiempo)
{
    for (int i = 0; i < Machine->numCPUs; i++)
    {
        for (int j = 0; j < Machine->cpus[i].numCores; j++)
        {
            for (int k = 0; k < Machine->cpus[i].cores[j].numThreads; k++)
            {
                if (Machine->cpus[i].cores[j].threads[k].ocupado == 1)
                {
                     Machine->cpus[i].cores[j].threads[k].pcb->tiempo_de_vida.tiempo_asignado -= tiempo;
                     if (Machine->cpus[i].cores[j].threads[k].pcb->tiempo_de_vida.tiempo_asignado <= 0)
                     {
                        Machine->cpus[i].cores[j].threads[k].ocupado = 0;
                     }
                     
                }
                

            }
        }
    }
}

void mostrarEstado(Machine  * Machine)
{
    for (int i = 0; i < Machine->numCPUs; i++)
    {
        for (int j = 0; j < Machine->cpus[i].numCores; j++)
        {
            for (int k = 0; k < Machine->cpus[i].cores[j].numThreads; k++)
            {
                printf("CPU: %d, Core: %d, Thread: %d, PCB: %d,Tiempo asignado: %d\n", Machine->cpus[i].id, Machine->cpus[i].cores[j].id, Machine->cpus[i].cores[j].threads[k].id, Machine->cpus[i].cores[j].threads[k].pcb->identificador ,Machine->cpus[i].cores[j].threads[k].pcb->tiempo_de_vida.tiempo_asignado);
            }
        }
    }
}

void limpiarMaquina(ColaDeColas * coladecolas, Machine * Machine)
{
    for (int i = 0; i < Machine->numCPUs; i++)
    {
        for (int j = 0; j < Machine->cpus[i].numCores; j++)
        {
            for (int k = 0; k < Machine->cpus[i].cores[j].numThreads; k++)
            {
                if(Machine->cpus[i].cores[j].threads[k].pcb->estado == 2 || Machine->cpus[i].cores[j].threads[k].pcb->estado == 1)
                {
                    enqueue(&coladecolas->colas[Machine->cpus[i].cores[j].threads[k].pcb->prioridad], Machine->cpus[i].cores[j].threads[k].pcb);
                    Machine->cpus[i].cores[j].threads[k].ocupado = 0;
                }
            }
        }
    }
}

int insertarPCBenMaquina(Machine * Machine, PCB * pcb)
{
    for (int i = 0; i < Machine->numCPUs; i++)
    {
        for (int j = 0; j < Machine->cpus[i].numCores; j++)
        {
            for (int k = 0; k < Machine->cpus[i].cores[j].numThreads; k++)
            {
                if(Machine->cpus[i].cores[j].threads[k].ocupado == 0)
                {
                    pcb->estado = 2;
                    Machine->cpus[i].cores[j].threads[k].pcb = pcb;
                    Machine->cpus[i].cores[j].threads[k].ocupado = 1;
                    Machine->cpus[i].cores[j].threads[k].pcb->estado = 2;
                    return 1;
                }
            }
        }
    }
    return 0;
}