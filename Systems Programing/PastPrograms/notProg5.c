#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <fcntl.h>

#define MAX_QUESTIONS 100
#define MAX_LEN 256
#define TIME_LIMIT 15

int current_question = 0;
int score = 0;
int total_questions = 0;
int quiz_active = 1;

void handle_exit(int sig) {
    char response;
    printf("\nExit (y/N)? ");
    response = getchar();
    if (response == 'y' || response == 'Y') {
        quiz_active = 0;
        printf("Exiting quiz...\n");
        exit(0);
    } else {
        // Clear input buffer
        while (getchar() != '\n');
    }
}

void handle_timer(int sig) {
    printf("\nTime has elapsed.\n");
    current_question++;
}

void start_timer() {
    struct itimerval timer;
    timer.it_value.tv_sec = TIME_LIMIT;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
}

int main() {
    char question[MAX_LEN];
    char answer[MAX_LEN];
    char user_input[MAX_LEN];
    struct sigaction sa;

    // Setup signal handler for Ctrl-C
    sa.sa_handler = handle_exit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    // Inform user about the quiz
    printf("You are about to begin a timed quiz. You will have %d seconds to answer each question.\n", TIME_LIMIT);
    printf("Press Ctrl-C to exit the program at any time.\n");
    printf("Do you wish to proceed? (y/N): ");
    char proceed;
    proceed = getchar();
    if (proceed != 'y' && proceed != 'Y') {
        printf("Exiting quiz...\n");
        return 0;
    }
    getchar();  // Clear newline from input buffer

    // Open question and answer files
    int q_fd = open("quest.txt", O_RDONLY);
    int a_fd = open("ans.txt", O_RDONLY);
    if (q_fd < 0 || a_fd < 0) {
        perror("Error opening question or answer file");
        return 1;
    }

    // Read questions and answers
    while ((read(q_fd, question, sizeof(question) - 1) > 0) && (read(a_fd, answer, sizeof(answer) - 1) > 0)) {
        question[strcspn(question, "\n")] = '\0';  // Remove newline
        answer[strcspn(answer, "\n")] = '\0';    // Remove newline
        total_questions++;

        // Display question and wait for answer
        while (quiz_active && current_question == total_questions - 1) {
            start_timer();
            printf("\nQuestion %d: %s\n", current_question + 1, question);
            printf("Your answer: ");
            fflush(stdout);
            alarm(TIME_LIMIT);
            fgets(user_input, sizeof(user_input), stdin);
            alarm(0);  // Cancel the alarm

            // Check answer
            user_input[strcspn(user_input, "\n")] = '\0';  // Remove newline
			answer[strcspn(answer, "\n")] = '\0';    // Remove newline
            if (user_input == answer) 
			{
                printf("Correct!\n");
                score++;
            } 
			else 
			{
                printf("Incorrect. The correct answer was: %s\n", answer);
            }
            current_question++;
        }
    }

    // Cleanup
    close(q_fd);
    close(a_fd);

    // Display results
    printf("\nQuiz completed! Your score: %d out of %d\n", score, total_questions);
    return 0;
}