#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h> 

#define BUFFER_SIZE 100

int errorCheck(int val, const char *msg)
{
	if (val == -1)
	{
		perror(msg);
		exit(EXIT_FAILURE);
	}
	return val;
}

int main() {
	//File descriptors for the pipe
	int pipe_fd[2];
	pid_t pid;
	char buffer[BUFFER_SIZE];
	
	//Create Pipe
	errorCheck(pipe(pipe_fd), "pipe");
	
	pid = fork();
	
	if (pid < 0) 
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	
	if (pid > 0)
	{
		//parent process
		close(pipe_fd[0]);
		
		while(1)
		{
			printf("Please enter text at the parent: ");
			fflush(stdout);
			if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
			{
				break;
			}
			if (strncmp(buffer, "exit", 4) == 0)
			{
				break;
			}
			else{
			errorCheck(write(pipe_fd[1], buffer, strlen(buffer)), "write");
			}
			sleep(1);
		}
		close(pipe_fd[1]);
		wait(NULL);
		
	}
	else
	{
		//child process
		close(pipe_fd[1]);
		
		while(1)
		{
			ssize_t bytes_read = read(pipe_fd[0], buffer, BUFFER_SIZE);
			if (bytes_read <= 0)
			{
				break;
			}
			else{
			printf("child: %s", buffer);
			}
			
		}
		close(pipe_fd[0]);
		exit(EXIT_SUCCESS);
		
	}
	return 0;
	
}
	
