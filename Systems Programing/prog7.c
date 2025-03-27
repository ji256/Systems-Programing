#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

int lower_bound = 1, upper_bound = 100;
int correct_number;
pid_t player1_pid, player2_pid;



// Global flags for signal handling
volatile sig_atomic_t player1_ready = 0;
volatile sig_atomic_t player2_ready = 0;
int player1_guess = 0;
int player2_guess = 0;

void parent_signal_handler(int sig) 
{
    switch (sig)
	{
        case SIGUSR1: // Player 1 has guessed
            player1_ready = 1;
            break;
        case SIGUSR2: // Player 2 has guessed
            player2_ready = 1;
            break;
        case SIGINT: // Terminate the game
            kill(player1_pid, SIGTERM);
            kill(player2_pid, SIGTERM);
            wait(NULL);
            wait(NULL);
            exit(0);
        case SIGCHLD: // Handle child termination
            wait(NULL);
            break;
    }
}

void child_signal_handler(int sig) 
{
    switch (sig) 
	{
        case SIGUSR1: // Player 1 received feedback
            player1_guess = (lower_bound + upper_bound) / 2;
            break;
        case SIGUSR2: // Player 2 received feedback
            player2_guess = rand() % (upper_bound - lower_bound + 1) + lower_bound;
            break;
        case SIGINT: // Restart guessing
            lower_bound = 1;
            upper_bound = 100;
            break;
        case SIGTERM: // Terminate child process
            exit(0);
    }
}

void player_guess(int player_id) 
{
    while (1) 
	{
        if (player_id == 1) 
		{
            // Player 1 guesses the average
            printf("Player 1 guesses: %d\n", player1_guess);
            kill(getppid(), SIGUSR1); // Notify parent with guess
        }
		else 
		{
            // Player 2 guesses randomly
            printf("Player 2 guesses: %d\n", player2_guess);
            kill(getppid(), SIGUSR2); // Notify parent with guess
        }

        pause(); // Wait for feedback from parent
    }
}

void notify_players() {
    // Notify both players to start guessing
    player1_guess = (lower_bound + upper_bound) / 2; // Set initial guess for Player 1
    player2_guess = rand() % (upper_bound - lower_bound + 1) + lower_bound; // Set initial guess for Player 2
    kill(player1_pid, SIGUSR1);
    kill(player2_pid, SIGUSR2);
}

int main() {
    // Set up signal handling for parent
    signal(SIGUSR1, parent_signal_handler);
    signal(SIGUSR2, parent_signal_handler);
    signal(SIGINT, parent_signal_handler);
    signal(SIGCHLD, parent_signal_handler);

    // Fork players
    player1_pid = fork();
    if (player1_pid == 0) {
        // Child process for Player 1
        signal(SIGUSR1, child_signal_handler);
        signal(SIGUSR2, child_signal_handler);
        signal(SIGINT, child_signal_handler);
        signal(SIGTERM, child_signal_handler);
        player_guess(1);
        exit(0);
    }

    player2_pid = fork();
    if (player2_pid == 0) {
        // Child process for Player 2
        signal(SIGUSR1, child_signal_handler);
        signal(SIGUSR2, child_signal_handler);
        signal(SIGINT, child_signal_handler);
        signal(SIGTERM, child_signal_handler);
        player_guess(2);
        exit(0);
    }

    // Main game loop
    srand(time(NULL));
    for (int round = 0; round < 10; round++) 
	{
        correct_number = rand() % 100 + 1;
        printf("Round %d: Correct number is %d\n", round + 1, correct_number);

        // Reset bounds for players
        lower_bound = 1;
        upper_bound = 100;

        // Notify players to start guessing
        notify_players();

        while (true) 
		{
            if (player1_ready && player2_ready) 
			{
                player1_ready = player2_ready = 0;

                // Check guesses
                if (player1_guess == correct_number) 
				{
                    printf("Player 1 guessed correctly!\n");
                    kill(player1_pid, SIGTERM);
                    break; // End round
                } 
				else if (player2_guess == correct_number) 
				{
                    printf("Player 2 guessed correctly!\n");
                    kill(player2_pid, SIGTERM);
                    break; // End round
                } 
				else 
				{
                    // Provide feedback
                    if (player1_guess < correct_number) 
					{
                        printf("Player 1's guess is too high.\n");
                        lower_bound = player1_guess + 1;
                    }
					else 
					{
                        printf("Player 1's guess is too low.\n");
                        upper_bound = player1_guess - 1;
                    }
                    
                    if (player2_guess < correct_number) 
					
					{
                        printf("Player 2's guess is too high.\n");
                        lower_bound = player2_guess + 1;
                    }
					else 
					{
                        printf("Player 2's guess is too low.\n");
                        upper_bound = player2_guess - 1;
                    }
                }

                // Notify players to update their guesses
                notify_players();
            }
            sleep(1); // Prevent busy-waiting
        }
    }

    // Cleanup and exit
    kill(player1_pid, SIGTERM);
    kill(player2_pid, SIGTERM);
    wait(NULL);
    wait(NULL);
    return 0;
}