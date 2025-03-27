#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>


#define packetSIZE 20

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
	int outFD1, outFD2, outFD3;
	ssize_t bRead;
	double val;
	unsigned char buffer[packetSIZE];
	
	short S;
	
	mkdir("/mnt/c/Documents/Systems Programing/values", S_IRUSR | S_IWUSR | S_IXUSR);
	
	//Open data file for reading
	fd = errorCheck(open("newRaw.dat", O_RDONLY), "open for read");
	outFD1 = errorCheck(open("accl.dat",O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR),"open for write");
	while ((bRead = read(fd,buffer,packetSIZE)) > 0)
	{
		printf("Acceleration:\n_______________________\n");
		S = ((short) buffer[3]) | buffer[2];
		val = S;
		printf("ax: %lf\n",val);
		write(outFD1, &val, sizeof(double));
	}
	close(fd);
	close(outFD1);
	
	fd = errorCheck(open("newRaw.dat", O_RDONLY), "open for read");
	outFD2 = errorCheck(open("rota.dat",O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR),"open for write");
	
	while ((bRead = read(fd,buffer,packetSIZE)) > 0)
	{
		
		printf("Angular:\n_______________________\n");
		
		write(outFD2, &val, sizeof(double));
		

	}
	close(fd);
	close(outFD2);
	
	fd = errorCheck(open("newRaw.dat", O_RDONLY), "open for read");
	outFD3 = errorCheck(open("angl.dat",O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR),"open for write");
	
	while ((bRead = read(fd,buffer,packetSIZE)) > 0)
	{
		
		printf("Angle:\n_______________________\n");
		
		write(outFD3, &val, sizeof(double));
	}
	close(fd);
	close(outFD3);
	
	
	exit(EXIT_SUCCESS);
}