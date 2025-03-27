#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_PATH "/tmp/myfifo"
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
	int fifo_fd;
	pid_t pid;
	char buffer[BUFFER_SIZE];
	
	// Create the FIFO 
    if (access(FIFO_PATH, F_OK) == -1) {
        errorCheck(mkfifo(FIFO_PATH, 0666), "mkfifo");
    }
	
	pid = fork();
	
	if (pid < 0) 
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	
	if (pid > 0)
	{
		//parent process open fifo for writing
		fifo_fd = open(FIFO_PATH, O_WRONLY);
		errorCheck(fifo_fd, "open for write");
		
		while(1)
		{
			printf("Please enter text at the parent: ");
			fflush(stdout);
			//read input of user
			if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
			{
				break;
			}
			//check to stop loop
			if (strncmp(buffer, "exit", 4) == 0)
			{
				break;
			}
			else{
			//write to fifo
			errorCheck(write(fifo_fd, buffer, strlen(buffer)), "write");
			//buffer[BUFFER_SIZE] = '\0';
			}
			//delay
			sleep(1);
		}
		close(fifo_fd);
		//wait(NULL);
		
	}
	else
	{
		//child proces open fifo for reading
		fifo_fd = open(FIFO_PATH, O_RDONLY);
		errorCheck(fifo_fd, "open for read");
		
		while(1)
		{
			ssize_t bytes_read = read(fifo_fd, buffer, BUFFER_SIZE);
			if (bytes_read <= 0)
			{
				break;
			}
			
			printf("child: %s", buffer);
			memset(buffer, 0, BUFFER_SIZE);    //ensures the buffer is cleared after it is printed
			
		}
		close(fifo_fd);
		exit(EXIT_SUCCESS);
		
	}
	return 0;
	
}