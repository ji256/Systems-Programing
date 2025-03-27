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
#include <sys/types.h>
#include <sys/wait.h>


#define packetSIZE 3
pid_t child;


int errorCheck (int val, const char *msg)
{
	if (val == -1)
	{
		perror(msg);
		exit(EXIT_FAILURE);
	}
	return val;
}


void signalHandler(int sig)      //signal handler for all signals
{
	
	
	if (sig == SIGUSR1)
	{
		printf("Warning! roll outside of bounds\n");     //roll message
	}
	else if (sig == SIGUSR2)
	{
		printf("Warning! pitch outside of bounds\n");	//pitch message
	}
	else if (sig == SIGCHLD)
	{
		pid_t pid;
		while ((pid = waitpid(-1, NULL, WNOHANG))>0)
		{
			printf("Child %d terminated.\n", pid);		//child is terminated
		}
	}
	else if (sig == SIGINT)								//exit handler
	{
		char response;
		printf("Exit: Are you sure (Y/N)?");
		fflush(stdout);
		response = getchar();
		if (response == 'Y' || response == 'y')
		{
			kill(child, SIGTERM);
			exit(EXIT_SUCCESS);
		}
		else
		{
			printf("Continuing...\n");
		}
	}

}
void clean()						//termiants any child left running
{
	
	if (child > 0)
	{
		kill(child,SIGTERM);
	}
}

void childProcess()					//child process of reading the roll & pitch values
{
	int fd;
	ssize_t bRead;
	double buffer[packetSIZE];
	
	
	
	fd = errorCheck(open("angl.dat", O_RDONLY), "open for read");
	
	
	while ((bRead = read(fd,buffer,3*sizeof(double))) > 0)
	{
		sleep(1);
		//roll value being read
		if(buffer[0] < -20.0 || buffer[0] > 20.0)
		{
			
			kill(getpid(), SIGUSR1);
		}
		//pitch value being read
		if(buffer[1] < -20.0 || buffer[1] > 20.0)
		{
			kill(getpid(), SIGUSR2);
		}
		
	}
	close(fd);
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	
	
	struct sigaction sa_chld, sa_int, sa_usr1, sa_usr2;
	
	sa_chld.sa_handler = signalHandler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = 0;
    sigaction(SIGCHLD, &sa_chld, NULL);

    sa_int.sa_handler = signalHandler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_usr1.sa_handler = signalHandler;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    sa_usr2.sa_handler = signalHandler;
    sigemptyset(&sa_usr2.sa_mask);
    sa_usr2.sa_flags = 0;
    sigaction(SIGUSR2, &sa_usr2, NULL);
	
	child = errorCheck(fork(), "Fork");
	
	if (child > 0)
	{
		sigset_t mask;
		sigemptyset(&mask);
		sigaddset(&mask, SIGINT);
		sigprocmask(SIG_BLOCK, &mask, NULL);
		
		childProcess();
	}
	
	//atexit(clean);
	printf("Parent created child: %d\n", child);
	
	while(1)			//pause for signals
	{
		pause();
	}
	
	exit(EXIT_SUCCESS);
}