#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <getopt.h>      
#include "header.h"

int main (int argc, char** argv) {
    FILE *fpb;
    MyRecord rec;
    long lSize;
    int numOfrecords, i;
    int searchId = -1;
    int amount = -1;
    int sleepTime = 0;
    int shmid = -1;
    char *filename = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "f:l:v:d:s:")) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            case 'l':
                searchId = atoi(optarg);
                break;
            case 'v':
                amount = atoi(optarg);
                break;
            case 'd':
                sleepTime = atoi(optarg);
                if (sleepTime > 20) {
                    sleepTime = 20;
                }
                break;
            case 's':
                shmid = atoi(optarg);
                break;
            default:
                printf("Correct syntax is: %s -f filename -l recid[,recid] -d time -s shmid\n", argv[0]);
                return 1;
        }
    }

    if (filename == NULL || searchId == -1 || sleepTime == 0 || shmid == -1 || amount == -1) {
        printf("Missing required arguments\n");
        return 1;
    }
    
    int *shared_memory = (int *)shmat(shmid, NULL, 0); //attaching to the shared memory

    if (shared_memory == (int *)(-1)) {
        perror("shmat failed");
        return 1;
    }

    fpb = fopen(filename, "rb+");
    if (fpb == NULL) {
        printf("Cannot open binary file\n");
        return 1;
    }

    //check number of records
    fseek(fpb, 0, SEEK_END);
    lSize = ftell(fpb);
    rewind(fpb);
    numOfrecords = (int)lSize / sizeof(rec);


    sem = sem_open("/mysemaphore", O_RDWR);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    //wait on the semaphore
    if (sem_wait(sem) == -1) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    printf("Entered critical section\n");
    
    //increase the number of writers
    shared_memory[2]++;

    for (i = 0; i < numOfrecords; i++) {
        fread(&rec, sizeof(rec), 1, fpb);
        if (rec.custid == searchId) {
            printf("balance is %d\n", rec.balance);
            //update the balance
            rec.balance += amount;

            printf("new balance is %d\n", rec.balance);
            //update the statistics
            shared_memory[3] = (shared_memory[3] + sleepTime);
            shared_memory[5]++;
            if (sleepTime > shared_memory[4]) {
                shared_memory[4] = sleepTime;
            }

            sleep(sleepTime);
            printf("%d %s %s  %d \n", \
                rec.custid, rec.LastName, \
                rec.FirstName, rec.balance);

            //write the updated record back to the file
            fseek(fpb, -sizeof(rec), SEEK_CUR);
            fwrite(&rec, sizeof(rec), 1, fpb);
            break;
        }
    }

    //release the semaphore
    if (sem_post(sem) == -1) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }

    //detaching the shared memory
    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed");
        return 1;
    }

    fclose (fpb);

    return 0;
}
