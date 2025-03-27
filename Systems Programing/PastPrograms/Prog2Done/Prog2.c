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
	int outFD;
	ssize_t bRead;
	double val;
	unsigned char buffer[packetSIZE];
	
	short S;
	
	//Open data file for reading
	fd = errorCheck(open("raw.dat", O_RDONLY), "open for read");
	outFD = errorCheck(open("newRaw.dat",O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR),"open for write");
	
	while ((bRead = read(fd,buffer,packetSIZE)) > 0)
	{
		//  val     L   H
		//   ax     2   3
		//   ay     4   5ls
		//   az     6   7
		//   wx     8   9
		//   wy    10  11
		//   wz    12  13
		// roll    14  15
		//pitch    16  17
		//  yaw    18  19
		// scaling -- a = 16, w = 2000, angles = 180
		//  f5 07 ... 
		// (short) 07 == 0007
		// 0007 << 8 == 0700
		// 0700 | f5 == 07f5
		printf("Acceleration:\n_______________________\n");
		S = ((short) buffer[3] << 8) | buffer[2];
		val = S/32768.0 * 16;
		printf("ax: %lf\n",val);
		
		S = ((short) buffer[5] << 8) | buffer[4];
		val = S/32768.0 * 16;
		printf("ay: %lf\n",val);
		
		S = ((short) buffer[6] << 8) | buffer[7];
		val = S/32768.0 * 16;
		printf("az: %lf\n\n",val);
		
		
		printf("Angular:\n_______________________\n");
		S = ((short) buffer[9] << 8) | buffer[8];
		val = S/32768.0 * 2000;
		printf("wx: %lf\n",val);
		
		S = ((short) buffer[11] << 8) | buffer[10];
		val = S/32768.0 * 2000;
		printf("wy: %lf\n",val);
		
		S = ((short) buffer[13] << 8) | buffer[12];
		val = S/32768.0 * 2000;
		printf("wz: %lf\n\n",val);
		
		printf("Angle:\n_______________________\n");
		S = ((short) buffer[15] << 8) | buffer[14];
		val = S/32768.0 * 180;
		printf("roll: %lf\n",val);
		
		S = ((short) buffer[17] << 8) | buffer[16];
		val = S/32768.0 * 180;
		printf("pitch: %lf\n",val);
		
		S = ((short) buffer[18] << 8) | buffer[19];
		val = S/32768.0 * 180;
		printf("yaw: %lf\n\n",val);
		write(outFD, &val, sizeof(double));
	}
	
	/*
	printf("Raw Data:\n");
	printf("_____________________________________\n");
	while((bRead = read(fd, buffer, packetSIZE)) > 0)
	{
		if (bRead < packetSIZE)
		{
			fprintf(stderr, "Warning: Incomplete data read\n");
		}
		for (int i = 0; i < bRead; i++)
		{
			printf("%02x", buffer[i]);
			
		}
		printf("\n");
	}
	

    // Process binary data to readable format
    printf("\nProcessed Data:\n");
    printf("_____________________________________\n");
    while(errorCheck((bRead = read(fd, &val, sizeof(double))), "process data"))
	{
		if(bRead == sizeof(double))
		{
			printf("%f\n", val);
		}
	}

	*/
	close(fd);
	exit(EXIT_SUCCESS);
}