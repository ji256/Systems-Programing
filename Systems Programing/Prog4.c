#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>


#define packetSIZE 3

int errorCheck (int val, const char *msg)
{
	if (val == -1)
	{
		perror(msg);
		exit(EXIT_FAILURE);
	}
	return val;
}

int main(int argc, char *argv[]) 
{
	int fd;
	ssize_t bRead;
	double buffer[packetSIZE];
	
	struct timespec req = {1,0}; //1 second interval

	
	fd = errorCheck(open("angl.dat", O_RDONLY), "open for read");
	
	printf("\nThese values are either within or outside the range -20.0 to 20.0.\n");
	printf("Being printed a 1 second interval of being read of each set of numbers.\n\n");
	
	while ((bRead = read(fd,buffer,3*sizeof(double))) > 0)
	{
		//roll value being read
		if(buffer[0] > -20.0 && buffer[0] < 20.0)
		{
			
			printf("roll: %lf is in range\n", buffer[0]);
		}
		else
		{
			printf("roll: %lf is not in range\n", buffer[0]);
		}
		
		//pitch value being read
		if(buffer[1] > -20.0 && buffer[1] < 20.0)
		{
			printf("pitch: %lf is in range\n", buffer[1]);
		}
		else
		{
			printf("pitch: %lf is not in range\n", buffer[1]);
		}
		
		//yaw value being read
		if(buffer[2] > -20.0 && buffer[2] < 20.0)
		{
			printf("yaw: %lf is in range\n", buffer[2]);
		}
		else
		{
			printf("yaw: %lf is not in range\n", buffer[2]);
		}
		printf("\n");
		//nanosleep() to read each set after 1 second
		nanosleep(&req, NULL);
	}
	close(fd);
	exit(EXIT_SUCCESS);
}