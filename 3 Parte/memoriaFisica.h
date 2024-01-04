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

typedef struct PhysicalMemory {
    char *memory;
    int start; // inicio de la memoria
    int end; // final de la memoria
    int currentPos; // posicion actual
    int textStart; // inicio de la seccion de texto
    int dataStart; // inicio de la seccion de datos
    int pageTableStart; // inicio de la seccion de la tabla de paginas
} PhysicalMemory;


PhysicalMemory* createPhysicalMemory() {
    PhysicalMemory* mem = (PhysicalMemory*)(sizeof (PhysicalMemory));
    mem->start = 0;
    mem->end = SIZE;
    mem->currentPos = 0;
    mem->textStart = 0; // inicio de la seccion de texto
    mem->dataStart = SIZE / 3; // inicio de la seccion de datos
    mem->pageTableStart = 2 * SIZE / 3; // inicio de la seccion de la tabla de paginas
    mem->memory = (char*) malloc(SIZE);
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

void writeText(PhysicalMemory* mem, int address, char value) {
    mem->memory[mem->textStart + address] = value;
}

void writeData(PhysicalMemory* mem, int address, char value) {
    mem->memory[mem->dataStart + address] = value;
}

void writePageTable(PhysicalMemory* mem, int address, char value) {
    mem->memory[mem->pageTableStart + address] = value;
}

void destroyPhysicalMemory(PhysicalMemory* mem) {

    free(mem);
}