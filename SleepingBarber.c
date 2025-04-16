#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define CHAIRS 3
#define CUSTOMERS 10
sem_t customers;
sem_t barber;
pthread_mutex_t accessSeats;
int freeSeats = CHAIRS;
void* barberFunc(void* arg) {
    while (1) {
        sem_wait(&customers);
        pthread_mutex_lock(&accessSeats); 
        freeSeats++; 
        printf("Barber is cutting hair. Free seats: %d\n", freeSeats);
        sem_post(&barber); 
        pthread_mutex_unlock(&accessSeats);
        sleep(3);
    }
}
void* customerFunc(void* num) {
    int id = *(int*)num;
    free(num);
    sleep(rand() % 5); 
    pthread_mutex_lock(&accessSeats);
    if (freeSeats > 0) {
        freeSeats--;
        printf("Customer %d takes a seat. Free seats: %d\n", id, freeSeats);

        sem_post(&customers);
        pthread_mutex_unlock(&accessSeats);

        sem_wait(&barber);
        printf("Customer %d is getting a haircut.\n", id);
    }
    else {
        printf("Customer %d leaves (no free seats).\n", id);
        pthread_mutex_unlock(&accessSeats);
    }
    pthread_exit(NULL);
}
int main() {
    pthread_t btid;
    pthread_t ctid[CUSTOMERS];
    sem_init(&customers, 0, 0);
    sem_init(&barber, 0, 0);
    pthread_mutex_init(&accessSeats, NULL);
    pthread_create(&btid, NULL, barberFunc, NULL);
    for (int i = 0; i < CUSTOMERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&ctid[i], NULL, customerFunc, id);
    }
    for (int i = 0; i < CUSTOMERS; i++) {
        pthread_join(ctid[i], NULL);
    }
    return 0;
}
