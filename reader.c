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

int main(int argc, char** argv) {
    FILE *fpb;
    MyRecord rec;
    long lSize;
    int numOfrecords, i;
    int startId, endId;
    int sleepTime;
    int shmid = -1;
    char* filename = NULL;
    char* recIds = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "f:l:d:s:")) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            case 'l':
                recIds = optarg;
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

    if (filename == NULL || recIds == NULL || sleepTime == 0 || shmid == -1) {
        printf("Missing required arguments\n");
        return 1;
    }

    int *shared_memory = (int *)shmat(shmid, NULL, 0); //attach the shared memory

    if (shared_memory == (int *)(-1)) {
        perror("shmat failed");
        return 1;
    }

    fpb = fopen(filename, "rb");
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

    //increase the readers
    shared_memory[0]++;
    //increase the total time reading
    shared_memory[1] = (shared_memory[1] + sleepTime);

    sleep(sleepTime);

    char* token = strtok(recIds, ",");
    while (token != NULL) {
        if (sscanf(token, "%d-%d", &startId, &endId) == 2) {
            //process multiple records in the specified range
            float total = 0;
            int counter = 0;

            for (i = 0; i < numOfrecords; i++) {
                fread(&rec, sizeof(rec), 1, fpb);
                if (rec.custid >= startId && rec.custid <= endId) { 
                    shared_memory[5]++;
                    if(sleepTime > shared_memory[4]) {
                        shared_memory[4] = sleepTime;
                    }
                    printf("%d %s %s  %d \n", \
                    rec.custid, rec.LastName, \
                            rec.FirstName, rec.balance);
                    
                    total += (float)rec.balance;
                    counter++;
                }
            }

            printf("Average balance: %.2f\n", total/counter);

        } else {
            //process a single record based on the provided ID
            int singleId = atoi(token);
            for (i = 0; i < numOfrecords; i++) {
                fread(&rec, sizeof(rec), 1, fpb);
                if (rec.custid == singleId) {
                    shared_memory[5]++;
                    if(sleepTime > shared_memory[4]) {
                        shared_memory[4] = sleepTime;
                    }
                    printf("%d %s %s  %d \n", \
                    rec.custid, rec.LastName, \
                            rec.FirstName, rec.balance);
                    break;
                    
                }
            }
        }
        token = strtok(NULL, ",");
    }

    //release the semaphore
    if (sem_post(sem) == -1) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }

    //detach the shared memory
    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed");
        return 1;
    }

    fclose(fpb);
    return 0;
}
