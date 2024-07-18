#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include "header.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <segment_id>\n", argv[0]);
        return 1;
    }

    int shmid = atoi(argv[1]); //get the segment ID from command line argument

    int *shared_memory = (int *)shmat(shmid, NULL, 0); //attach the shared memory

    if (shared_memory == (int *)(-1)) {
        perror("shmat failed");
        return 1;
    }

    //statistics
    printf("Number of readers: %d \n", shared_memory[0]);
    if (shared_memory[0] == 0) {
        printf("Average time reading: 0 \n");
    } else {
        printf("Average time reading: %d \n", shared_memory[1]/shared_memory[0]);
    }
    printf("Number of writers: %d \n", shared_memory[2]);
    if (shared_memory[2] == 0) {
        printf("Average time writing: 0 \n");
    } else {
        printf("Average time writing: %d \n", shared_memory[3]/shared_memory[2]);
    }
    printf("Maximum time reading/writing: %d \n", shared_memory[4]);
    printf("Number of records read/written: %d \n", shared_memory[5]);

    //detach the shared memory
    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed");
        return 1;
    }

    //remove the shared memory segment
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID failed");
        return 1;
    }

    sem_destroy(sem);

    return 0;
}
