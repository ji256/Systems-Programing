#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>

struct shared_data {
    int num_blocks;          // Number of blocks
    int lengths[20];         // Lengths of each block (max 20 blocks)
    char characters[20];     // Characters for each block (max 20 blocks)
};

void sem_wait(int semid, int index) {
    struct sembuf sb = {index, -1, 0};
    semop(semid, &sb, 1);
}

void sem_signal(int semid, int index) {
    struct sembuf sb = {index, 1, 0};
    semop(semid, &sb, 1);
}

void child_process(int semid, int shmid) {
    struct shared_data *data = (struct shared_data *)shmat(shmid, NULL, 0);
    srand(time(NULL)); // Seed the random number generator

    sem_wait(semid, 0); // Reserve child semaphore

    // Generate random data
    data->num_blocks = rand() % 11 + 10; 
    for (int i = 0; i < data->num_blocks; i++) {
        data->lengths[i] = rand() % 9 + 2; 
        data->characters[i] = rand() % 26 + 'a'; 
    }

    sem_signal(semid, 1); // Release parent semaphore
    sem_wait(semid, 0); // Wait for parent to finish

    shmdt(data); // Detach shared memory segment
    exit(0);
}

void parent_process(int semid, int shmid) {
    struct shared_data *data = (struct shared_data *)shmat(shmid, NULL, 0);
    srand(time(NULL)); 

    sem_wait(semid, 1); // Reserve parent semaphore

    // Generate random width for ASCII art
    int width = rand() % 6 + 10; // Between 10 and 15
    int current_length = 0;

    // Output the ASCII art
    for (int i = 0; i < data->num_blocks; i++) {
        for (int j = 0; j < data->lengths[i]; j++) {
            putchar(data->characters[i]);
            current_length++;
            if (current_length >= width) {
                putchar('\n');
                current_length = 0;
            }
        }
    }
    if (current_length > 0) {
        putchar('\n'); // New line if there are leftover characters
    }

    sem_signal(semid, 0); 
    shmdt(data); 
}

int main(int argc, char *argv[]) {
    // Create semaphore set
    int semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 1); 
    semctl(semid, 1, SETVAL, 0); 

    // Create shared memory segment
    int shmid = shmget(IPC_PRIVATE, sizeof(struct shared_data), IPC_CREAT | 0666);

    // Create child process
    if (fork() == 0) {
        child_process(semid, shmid);
    } else {
        parent_process(semid, shmid);
    }

    // Clean up
    semctl(semid, 0, IPC_RMID);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}