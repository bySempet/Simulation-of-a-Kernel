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

typedef struct {
    char memory[SIZE];
} PhysicalMemory;

int read(PhysicalMemory* mem, int address) {

    return 0;
}

void write(PhysicalMemory* mem, int address, int value) {

}

PhysicalMemory* createPhysicalMemory() {
    PhysicalMemory* mem = malloc(sizeof(PhysicalMemory));

    return mem;
}

void destroyPhysicalMemory(PhysicalMemory* mem) {

    free(mem);
}