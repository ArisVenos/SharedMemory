#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include "header.h"

int main() {
    key_t key = ftok(".", 'x'); //generating a key

    if (key == -1) {
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }

    int shmid = shmget(key, SEGMENT_SIZE, IPC_CREAT | 0666); //create a shared memory segment

    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    int *shared_memory = (int *)shmat(shmid, NULL, 0); //attach the shared memory

    if (shared_memory == (int *)(-1)) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    //write the initial values to the shared memory
    int numbers[6] = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 6; ++i) {
        shared_memory[i] = numbers[i];
    }

    sem = sem_open("/mysemaphore", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    printf("Shared memory segment ID: %d\n", shmid);

    //detach the shared memory
    shmdt(shared_memory);

    return 0;
}
