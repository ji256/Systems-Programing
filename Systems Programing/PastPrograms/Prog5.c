#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <string.h>





int errorCheck (int val, const char *msg)
{
	if (val == -1)
	{
		perror(msg);
		exit(EXIT_FAILURE);
	}
	return val;
}


void timerHandler(int sigNum)
{
	printf("Time has elapsed.\n");
}

void signalHandler(int sig)
{
	char leave;
	
	signal(sig, SIG_IGN);
	printf("\nExit (y/n): ");
	leave = getchar();
	if ( leave == 'y' || leave == 'Y')
	{
		printf("Exiting Quiz....\n");
		exit(0);
	}
	else
	{
		signal(SIGINT, signalHandler);
		while(getchar() != '\n');
	}
	
}

int main(int argc, char *argv[]) 
{
	int q_fd;
	int a_fd;
	int i;
	char ch;
	ssize_t bRead;
	char proceed;
	int total_questions = 0;
	char question[200];
    char answer[200];
    char user_input[200];
	int score = 0;

	
	struct itimerval it;
	
	it.it_interval.tv_sec = 15;
	it.it_interval.tv_usec = 0;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 0;
	//setitimer(ITIMER_REAL, &it, NULL);
	
	
	if (setitimer(ITIMER_REAL, &it, NULL) == -1)
	{
		perror("setitimer");
		exit(EXIT_FAILURE);
	}
	
	struct sigaction sa;
	
	sa.sa_handler = timerHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    //sigaction(SIGALRM, &sa, NULL);
	
	if (sigaction(SIGALRM, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	
	q_fd = errorCheck(open("quest.txt", O_RDONLY), "open for read");
	a_fd = errorCheck(open("ans.txt", O_RDONLY), "open for read");

	
	printf("You are about to begin a timed quiz.\nYou will have 15 seconds to answer each question.\n");
	printf("Press Ctrl-C to exit at any time.\n");
	printf("Do you wish to proceed? (y/n): ");
	proceed = getchar();
	
	if ( proceed != 'y' && proceed != 'Y')
	{
		printf("Exiting...\n");
		exit(0);
	}
	
	while ((((bRead = read(q_fd,question,200*sizeof(char)-1)) > 0)) && (((bRead = read(a_fd,answer,200*sizeof(char)-1)) > 0)))
	{
		if(signal(SIGINT, signalHandler) == SIG_ERR);
			
		
		
		printf("\nQuestion 1:\n\n");
		for (int i = 0; i < 8; i++)
		{
			printf("%c", question[i]);
		}
		printf("\n");
		printf("Your Answer: ");
		fflush(stdout);
		scanf("%199s", user_input);
		user_input[strcspn(user_input, "\n")] = '\0';
		
		
		printf("\nQuestion 2:\n");
		for (int i = 9; i < 31; i++)
		{
			
			printf("%c", question[i]);
		}
		printf("\n");
		printf("Your Answer: ");
		fflush(stdout);
		scanf("%199s", user_input);
		
		
		printf("\nQuestion 3:\n");
		for (int i = 32; i < 65; i++)
		{
			
			printf("%c", question[i]);
		}
		printf("\n");
		printf("Your Answer: ");
		fflush(stdout);
		scanf("%199s", user_input);
		
		
		
		
		
	}
	
	
	
	
	
	close(q_fd);
	close(a_fd);
	exit(EXIT_SUCCESS);
}