#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

int main(int argc, char *argv[])
{
	int fd;
	int return_v;
	int buf;
	char stringToSend[255] = {0};
	char *myfifo = "/tmp/myfifo";
	fd = open(myfifo, O_RDONLY);
	if(fd < 0)
	{
		printf("Failed to open tmp/myfifo...");
		return fd;
   	}
	read(fd, &buf, sizeof(int));
	printf("P3 Received: %d\n", buf);
	close(fd);

	sprintf(stringToSend,"%d", buf);
	fd = open("/proc/D1", O_RDWR);             // Open the device with read/write access
	if(fd < 0)
	{
		printf("Failed to open the device...");
		return fd;
   	}
 
	return_v = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
	if (return_v < 0)
	{
		printf("Failed to write the message to the device.");
		return return_v;
	}

	return (0);
}
