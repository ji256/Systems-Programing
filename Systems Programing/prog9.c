#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define NUM_GAMES 10

// Global variables
int guess[2]; 
int cmp[2];   
int rdy[4];   
pthread_mutex_t mtx[3]; 
pthread_cond_t cnd[3];  

// Game stats
int player1_score = 0;
int player2_score = 0;

void* player1(void* arg) {
    int min, max;
    for (int game = 0; game < NUM_GAMES; game++) {
        min = 0;
        max = 100;

        pthread_mutex_lock(&mtx[2]);
        while (rdy[2] == 0) {
            pthread_cond_wait(&cnd[2], &mtx[2]);
        }
        rdy[2] = 0; // Reset readiness flag
        pthread_mutex_unlock(&mtx[2]);

        while (1) {
            guess[0] = (min + max) / 2; // Average guess
            pthread_mutex_lock(&mtx[0]);
            pthread_cond_wait(&cnd[0], &mtx[0]);
            pthread_mutex_unlock(&mtx[0]);

            if (cmp[0] == 0) break; // Correct guess
            if (cmp[0] < 0) min = guess[0]; // Update min
            else max = guess[0]; // Update max
        }
    }
    return NULL;
}
/*
My player 2 just kept guessing 0 and I couldn't figure out why.
My debug statements where not printing at all.
*/
void* player2(void* arg) {
    srand(time(NULL) + 1); // Seed random number generator
    int min, max;
    for (int game = 0; game < NUM_GAMES; game++) {
        min = 1;
        max = 100;

        pthread_mutex_lock(&mtx[2]);
        while (rdy[3] == 0) {
            pthread_cond_wait(&cnd[3], &mtx[2]);
        }
        rdy[3] = 0; // Reset readiness flag
        pthread_mutex_unlock(&mtx[2]);

        while (1) {
			//debug
			printf("Player 2: min = %d, max = %d\n", min, max);
			fflush(stdout);
			
            guess[1] = rand() % (max - min + 1) + min; // Random guess
			
			//debug
			printf("Player 2 guesses: %d\n", guess[1]);
			fflush(stdout);
			
            pthread_mutex_lock(&mtx[1]);
			pthread_cond_signal(&cnd[1]);
            pthread_mutex_unlock(&mtx[1]);
			
			pthread_mutex_lock(&mtx[1]);
            pthread_cond_wait(&cnd[1], &mtx[1]);
            pthread_mutex_unlock(&mtx[1]);
			
			//debug
			printf("Player 2 comparison result: %d\n", cmp[1]);
            fflush(stdout);

            if (cmp[1] == 0) break; // Correct guess
            if (cmp[1] < 0) min = guess[1] + 1; // Update min
            else max = guess[1] - 1; // Update max
        }
    }
    return NULL;
}

void* referee(void* arg) {
    for (int game = 0; game < NUM_GAMES; game++) {
        pthread_mutex_lock(&mtx[2]);
        int target = rand() % 100 + 1; // Random target between 1 and 100
        printf("Game %d: Target is %d\n", game + 1, target);
        
        rdy[2] = 1; // Signal players to start
        rdy[3] = 1;
        pthread_cond_broadcast(&cnd[2]);
        pthread_mutex_unlock(&mtx[2]);

        sleep(1); // Wait for players to guess

        pthread_mutex_lock(&mtx[1]);
        rdy[0] = 0; // Reset readiness for player 1
        rdy[1] = 0; // Reset readiness for player 2

        cmp[0] = guess[0] - target; // Compare player 1's guess
        cmp[1] = guess[1] - target; // Compare player 2's guess

        rdy[0] = 1; // Signal player 1
        rdy[1] = 1; // Signal player 2
        pthread_cond_broadcast(&cnd[1]);
        pthread_cond_broadcast(&cnd[0]);
        pthread_mutex_unlock(&mtx[1]);
		
		printf("Player 1 guessed: %d, Player 2 guessed: %d\n", guess[0], guess[1]);

        // Check for correct guesses
        if (cmp[0] == 0) {
            player1_score++;
            printf("Player 1 guessed correctly!\n");
			fflush(stdout);
        }
        if (cmp[1] == 0) {
            player2_score++;
            printf("Player 2 guessed correctly!\n");
			fflush(stdout);
        }
    }
    printf("Final scores: Player 1 - %d, Player 2 - %d\n", player1_score, player2_score);
	fflush(stdout);
    return NULL;
}

int main() {
    srand(time(NULL)); // Seed random number generator

    // Initialize mutexes and condition variables
    for (int i = 0; i < 3; i++) {
        pthread_mutex_init(&mtx[i], NULL);
        pthread_cond_init(&cnd[i], NULL);
    }

    // Initialize readiness flags
    for (int i = 0; i < 4; i++) {
        rdy[i] = 0;
    }

    // Create threads
    pthread_t player1_thread, player2_thread, referee_thread;
    pthread_create(&player1_thread, NULL, player1, NULL);
    pthread_create(&player2_thread, NULL, player2, NULL);
    pthread_create(&referee_thread, NULL, referee, NULL);

    // Join referee thread
    pthread_join(referee_thread, NULL);

    return 0;
}