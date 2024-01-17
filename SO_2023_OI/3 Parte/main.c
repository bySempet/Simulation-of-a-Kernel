#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include "machine.h"
#include "memoriaFisica.h"


//Inicializar las variables globales

const int numColas = 100; //Declaramos la cantidad de colas que existiran, equivalentes al numero de prioridades
typedef struct Reloj{
    int segundos;
    pthread_cond_t cond;
} Reloj;

typedef struct MachineType{
    pthread_mutex_t mutex_maquina;
    pthread_cond_t cond;
} machineType;
Reloj* reloj; //Reloj de la maquina
machineType * machine; //Mutex
Machine* maquina; //Maquina
PhysicalMemory* memoriaFisica; //Memoria fisica
ColaDeColas* coladecolas; //Cola de colas
int cpus, cores, hilos = 0; //Variables para la creacion de la maquina
pthread_mutex_t mutex_reloj, mutex_timer; //Mutex para el reloj, la maquina y el timer
pthread_cond_t cond_timer; //Variable de condicion para el timer

//Funciones de los threads
//Va a generar todos los pcbs y los va a ir metiendo en las queue
//Proceso de tiempo limitado debido a que se generaran x PCBs
/*void *generador(void *arg)
{

    srand(time(NULL)); // Para generar números aleatorios
    int i = 1;
    while (1) 
   // {
   // for (int i = 1; i < 150; i++)
    {
        // Crear un nuevo PCB
        PCB* nuevoPCB = (PCB*)malloc(sizeof(PCB));
        nuevoPCB->identificador = i; // Establecer el identificador
        nuevoPCB->prioridad = rand() % 97 + 1; //Generar una prioridad aleatoria
        nuevoPCB->tiempo_de_vida.creacion = time(NULL); // Establecer el tiempo de creación
        nuevoPCB->tiempo_de_vida.tiempo_asignado = rand() % 70 +1; // Generar un tiempo de vida aleatorio
        nuevoPCB->estado = 0; // Generar un estado (0, 1, 2)
        // Insertar el PCB en la cola de prioridad adecuada
        pthread_mutex_lock(&machine->mutex_maquina);
        agregarAQueueDePrioridad(coladecolas,nuevoPCB); 
        pthread_mutex_unlock(&machine->mutex_maquina);
        i++;
        if(i % 10 == 0) sleep(5);
    }
  //  }

}*/

void *loader(void *arg)
{
    static int file_number = 0; // Variable estática que mantiene su valor entre las llamadas a la función
    while (1)
    {
    
        char filename[20];
        sprintf(filename, "prog%03d.elf", file_number);

        FILE *file = fopen(filename, "r");
        if (file == NULL)
        {
            printf("No se pudo abrir el archivo %s\n", filename);
            file_number++;
            continue;
        }

        int code_start_address, data_start_address;
        fscanf(file, ".text %d\n", &code_start_address);
        fscanf(file, ".data %d\n", &data_start_address);

        char *code = malloc(1000 * sizeof(char));
        char *data = malloc(1000 * sizeof(char));

        char line[10];
        int address = code_start_address;
        while (address < data_start_address && fgets(line, sizeof(line), file))
        {
            strcat(code, line);
            address += 4; // Cada línea representa 4 direcciones
        }

        while (fgets(line, sizeof(line), file))
        {
            strcat(data, line);
        }

        fclose(file);
        
        int pgb = writeInMemory(memoriaFisica, code, data);

        // Crear un nuevo PCB
        PCB* nuevoPCB = (PCB*)malloc(sizeof(PCB));
        nuevoPCB->identificador = file_number; // Establecer el identificador
        nuevoPCB->prioridad = rand() % 97 + 1; //Generar una prioridad aleatoria
        nuevoPCB->tiempo_de_vida.creacion = time(NULL); // Establecer el tiempo de creación
        nuevoPCB->tiempo_de_vida.tiempo_asignado = rand() % 70 +1; // Generar un tiempo de vida aleatorio
        nuevoPCB->estado = 0; // Generar un estado (0, 1, 2)
        mm * nuevoMM = (mm*)malloc(sizeof(mm));
        nuevoMM->code = code_start_address;
        nuevoMM->data = data_start_address;
        nuevoMM->pgb = pgb;
        nuevoPCB->mm = *nuevoMM;

        // Insertar el PCB en la cola de prioridad adecuada
        pthread_mutex_lock(&machine->mutex_maquina);
        agregarAQueueDePrioridad(coladecolas,nuevoPCB); 
        pthread_mutex_unlock(&machine->mutex_maquina);

        file_number++;
        if(file_number % 10 == 0) sleep(5);
        
    }
    return NULL;
}

void *timer(void *arg)
{
    while (1)
     {
        pthread_mutex_lock(&mutex_reloj);
        pthread_cond_wait(&reloj->cond, &mutex_reloj);

        printf("Reloj: %d segundos\n", reloj->segundos);
        if(reloj->segundos % 5 == 0)
        {
            pthread_mutex_lock(&mutex_timer);
            pthread_cond_signal(&cond_timer);
            restarTiempo(maquina, 5);
            mostrarEstado(maquina);
            pthread_mutex_unlock(&mutex_timer);
            printf("Temporizador: Se ha alcanzado un múltiplo de 5 segundos (%d segundos)\n", reloj->segundos);
        }
        pthread_mutex_unlock(&mutex_reloj);

    }
    
}
void *cloc(void *arg)
{

    while (1) 
    {
        sleep(1);
        pthread_mutex_lock(&mutex_reloj);
        reloj->segundos++;
        pthread_cond_signal(&reloj->cond);

        pthread_mutex_lock(&machine->mutex_maquina);
        pthread_cond_signal(&machine->cond);
        pthread_mutex_unlock(&machine->mutex_maquina);

        pthread_mutex_unlock(&mutex_reloj);

    }
}
void *scheduler(void *arg)
{
    while (1) {

        pthread_mutex_lock(&machine->mutex_maquina);
        // Esperar a que el cloc de señal cada segundi
        pthread_cond_wait(&machine->cond, &machine->mutex_maquina);

        // Rellenar la máquina con PCBs de la coladecolas
        int total = cores *cpus * hilos;
        for(int i=0; i<total; i++)
        {
            PCB* pcb = dequeueColas(coladecolas);
            if(insertarPCBenMaquina(maquina, pcb) == 0) 
            {
                printf("Scheduler: No se pudo insertar el PCB %d en la máquina\n", pcb->identificador);
                enqueue(&coladecolas->colas[pcb->prioridad], pcb);
            } 
        }

        pthread_mutex_unlock(&machine->mutex_maquina);

        pthread_mutex_lock(&mutex_timer);
        // Esperar a que el temporizador envíe una señal
        pthread_cond_wait(&cond_timer, &mutex_timer);
        // Quitar todos los procesos de la máquina y devolverlos a su cola correspondiente
        limpiarMaquina(coladecolas, maquina);

        // Llenar la máquina con PCBs de la coladecolas
        for(int i=0; i<total; i++)
        {
            PCB* pcb =  dequeueColas(coladecolas);
            if(insertarPCBenMaquina(maquina, pcb) == 0) 
            {
                printf("Scheduler: No se pudo insertar el PCB %d en la máquina\n", pcb->identificador);
                enqueue(&coladecolas->colas[pcb->prioridad], pcb);
            } 
           
        }

        pthread_mutex_unlock(&mutex_timer);
    }
}
//Funcion para inicializar y crear toda la "maquina"
void inizializarMaquina()
{
    //Recibir por terminal los datos necesarios
    printf("Para la creacion de la maquina se va a solicitar una serie de parametros \n");
    printf("Introduzca la cantidad que le plazca de cada uno de ellos \n");
    printf("Cuanto mayor sea el tamaño mas despacio correra la aplicacion \n");
    printf("Introduzca la cantidad de CPUs \n");
    scanf("%d",&cpus);
    printf("Introduzca la cantidad de CORES \n");
    scanf("%d",&cores);
    printf("Introduzca la cantidad de HILOS \n");
    scanf("%d",&hilos);
    //Inicializar la "maquina" 
    maquina = createMachine(cpus,cores,hilos);
    if(maquina == NULL) exit(EXIT_FAILURE);
    //Inicializar la memoria fisica
    memoriaFisica = createPhysicalMemory();
    //Inicializar la cola donde se guardaran los procesos
    coladecolas = inicializarColaDeColas(numColas);
    if(coladecolas == NULL) exit(EXIT_FAILURE);
    printf("Proceso de creacion de entorno correcto \n");
    //Inicializar el reloj
    reloj = (Reloj*)malloc(sizeof(Reloj));
    if(reloj == NULL) exit(EXIT_FAILURE);
    reloj->segundos = 0;
    machine = malloc(sizeof(machineType));
    pthread_cond_init(&reloj->cond, NULL);
    //Inicializar los mutex
    pthread_mutex_init(&mutex_reloj, NULL);
    pthread_mutex_init(&machine->mutex_maquina, NULL);
    pthread_mutex_init(&mutex_timer, NULL);
    //Inicializar la variable de condicion
    pthread_cond_init(&cond_timer, NULL);
    pthread_cond_init(&machine->cond, NULL);

}
//Funcion para inicializar los hilos

void inizializarHilos()
{
    pthread_t thread_generador, thread_timer, thread_scheduler, thread_cloc;
    int err;
  
    //Crear los threads
    err = pthread_create(&thread_generador, NULL, loader, 0);
    if (err != 0) printf("\nError al crear el hilo generador :[%s]", strerror(err));
    err = pthread_create(&thread_timer, NULL, timer, 0);
    if (err != 0) printf("\nError al crear el hilo timer :[%s]", strerror(err));
    err = pthread_create(&thread_scheduler, NULL, scheduler, 0);
    if (err != 0) printf("\nError al crear el hilo scheduler :[%s]", strerror(err));
    err = pthread_create(&thread_cloc, NULL, cloc, 0);
    if (err != 0) printf("\nError al crear el hilo cloc :[%s]", strerror(err));

}

//main
void main()
{

    inizializarMaquina();

    inizializarHilos();

    sleep(1000);

    destroyMachine(maquina);
    free(coladecolas);
    free(reloj);
    free(machine);
    pthread_mutex_destroy(&mutex_reloj);
    pthread_mutex_destroy(&machine->mutex_maquina);
    pthread_mutex_destroy(&mutex_timer);
    pthread_cond_destroy(&cond_timer);
    pthread_cond_destroy(&machine->cond);
    exit(EXIT_SUCCESS);

}