#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

bool mode;
int readent = 0;
int writent = 0;                  
sem_t mutex;                      
sem_t write,read;
pthread_t reader1,reader2,writer1,writer2;                  

void* reader(void* arg) {
    while (mode) {
        sem_wait(&mutex);
        readent++;
        if (readent == 1)
            sem_wait(&write); // First reader blocks writers
        sem_post(&mutex);

        //Reading
        printf("Reading\n");
        sem_wait(&mutex);
        readent--;
        if (readent == 0)
            sem_post(&write); // Last reader allows writers to proceed
        sem_post(&mutex);
    }
    while (!mode){
        sem_wait(&read);     // Wait for exclusive access
        // Reading
        printf("Reading\n");
        sem_post(&read);     // Release exclusive access
    }
    
    return NULL;
}

void* writer(void* arg) {
    while (mode) {
        sem_wait(&write);     // Wait for exclusive access
        // Writing
        printf("Writing\n");
        sem_post(&write);     // Release exclusive access
    }
    while(!mode){
        sem_wait(&mutex);
        writent++;
        if (writent == 1)
            sem_wait(&read); // First writer blocks readers
        sem_post(&mutex);

        //writing
        printf("writing\n");
        sem_wait(&mutex);
        writent--;
        if (writent == 0)
            sem_post(&read); // Last writer allows readers to proceed
        sem_post(&mutex);

    }
    return NULL;
}

int main() {
    sem_init(&mutex, 0, 1);
    sem_init(&write, 0, 1);
    sem_init(&read, 0, 1);
    mode = true;//set to true for priorety for reades and false for writers
    if (mode) 
        printf("Readers have priority\n");
    else 
        printf("Writers have priority\n");
        
    // Create reader threads
    pthread_create(&reader1, NULL, reader, NULL);
    pthread_create(&reader2, NULL, reader, NULL);

    // Create writer threads
    pthread_create(&writer1, NULL, writer, NULL);
    pthread_create(&writer2, NULL, writer, NULL);

    // Join threads (will never return in this infinite-loop example)
    pthread_join(reader1, NULL);
    pthread_join(reader2, NULL);
    pthread_join(writer1, NULL);
    pthread_join(writer2, NULL);

    return 0;
}
