#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define ADDRESS_BUS 24
#define WORD_SIZE 4
#define SIZE (1 << ADDRESS_BUS)

// Estructura para representar la memoria física
typedef struct PhysicalMemory {
    char *memory;
    int *ocupado;
    int start; // inicio de la memoria
    int end; // final de la memoria
    int currentPosText; // posicion actual texto
    int textStart; // inicio de la seccion de texto
    int currentPosData; // posicion actual datos
    int dataStart; // inicio de la seccion de datos
    int pageTableStart; // inicio de la seccion de la tabla de paginas
    int currentPosTable
} PhysicalMemory;


PhysicalMemory* createPhysicalMemory() {
    PhysicalMemory* mem = (PhysicalMemory*)malloc(sizeof (PhysicalMemory));
    mem->start = 0;
    mem->end = SIZE;
    mem->textStart = 0; // inicio de la seccion de texto
    mem->currentPosText = 0; // posicion actual texto
    mem->dataStart = SIZE / 3; // inicio de la seccion de datos
    mem->currentPosData = SIZE / 3; // posicion actual datos
    mem->pageTableStart = 2 * SIZE / 3; // inicio de la seccion de la tabla de paginas
    mem->currentPosTable = 2 * SIZE / 3;
    mem->memory = (char*) malloc(SIZE);
    mem->ocupado = (int*) malloc(SIZE);
    return mem;
}

char* getText(PhysicalMemory* mem, int address) {
    return &(mem->memory[mem->textStart + address]);
}

char* getData(PhysicalMemory* mem, int address) {
    return &(mem->memory[mem->dataStart + address]);
}

char* getPageTable(PhysicalMemory* mem, int address) {
    return &(mem->memory[mem->pageTableStart + address]);
}

void writeText(PhysicalMemory* mem,  char value) {
    mem->memory[mem->textStart + mem->currentPosText] = value;
    mem->ocupado[mem->textStart + mem->currentPosText] = 1;
}

void writeData(PhysicalMemory* mem, char value) {
    mem->memory[mem->dataStart + mem->currentPosData] = value;
    mem->ocupado[mem->dataStart + mem->currentPosData] = 1;
}

void writePageTable(PhysicalMemory* mem,  char value) {
    mem->memory[mem->pageTableStart + mem->currentPosTable] = value;
    mem->ocupado[mem->pageTableStart + mem->currentPosTable] = 1;
}
int writeInMemory(PhysicalMemory* mem, char *code, char *data) {
    if (mem->currentPosText = mem->dataStart) 
    {
        mem->currentPosText = 0;
        mem->memory[mem->currentPosText] = code;
        mem->ocupado[mem->currentPosText] = 1;
        mem->currentPosText++;
    }
    else
    {
        mem->memory[mem->currentPosText] = code;
        mem->ocupado[mem->currentPosText] = 1;
        mem->currentPosText++;
    }
    if (mem->currentPosData = mem->pageTableStart) 
    {
        mem->currentPosData = mem->dataStart;
        mem->memory[mem->currentPosData] = data;
        mem->ocupado[mem->currentPosData] = 1;
        mem->currentPosData++;
    }
    else
    {
        mem->memory[mem->currentPosData] = data;
        mem->ocupado[mem->currentPosData] = 1;
        mem->currentPosData++;
    }
    char posiciones[2];
    posiciones[0] = mem->currentPosText;
    posiciones[1] = mem->currentPosData;

    if (mem->currentPosTable = mem->end) 
    {
        mem->currentPosTable = mem->pageTableStart;
        mem->memory[mem->currentPosTable] = posiciones[0]+ ","+ posiciones[1];
        mem->ocupado[mem->currentPosTable] = 1;
        mem->currentPosTable++;  
    }
    else
    {
        mem->memory[mem->currentPosTable] = posiciones[0]+ ","+ posiciones[1];
        mem->ocupado[mem->currentPosTable] = 1;
        mem->currentPosTable++;
    }
    
    return mem->currentPosTable-1;
}
int isOccupiedTable(PhysicalMemory* mem) {
    return mem->ocupado[mem->pageTableStart + mem->currentPosTable];
}
int isOccupiedData(PhysicalMemory* mem) {
    return mem->ocupado[mem->dataStart + mem->currentPosData];
}
int isOccupiedText(PhysicalMemory* mem) {
    return mem->ocupado[mem->textStart + mem->currentPosText];
}
void destroyPhysicalMemory(PhysicalMemory* mem) {

    free(mem);
}