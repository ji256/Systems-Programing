#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
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

int main() {
    // Seed the random number generator
    srand(time(NULL));

    // Generate a random number between 0 and 255
    int random_number = rand() % 256;

    

    // Open the file for writing
    int fd = errorCheck(open("dataX.dat", O_CREAT | O_WRONLY, 0644), "opent for write");
   

    // Generate 60 random values between 0 and 100 and store them in the file
    for (int i = 0; i < 60; i++) {
        int value = rand() % 101;
        char buffer[10];
        sprintf(buffer, "%d ", value);
        write(fd, buffer, strlen(buffer));
    }

    // Close the file
    close(fd);

    // Exit with the random number as the status
    exit(random_number);
}


