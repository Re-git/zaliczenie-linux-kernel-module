#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

// Number x to send P1--unnamed_pipe-->P2--named_pipe-->p3-->

int main(int argc, char *argv[])
{

	int fd[2];
	int nbytes __attribute__((unused));
	int childpid;
	int readbuffer;

	pipe(fd);

	if((childpid = fork()) == -1) {
		perror("fork");
		exit(1);
	}
	if(childpid == 0) {
		int x = 0;
		close(fd[0]);
		printf("Podaj liczbe:");
		scanf("%d",&x);
		printf("P1 Sending number %d to pipe\n", x);
		write(fd[1], &x, sizeof(x));
		exit(0);
	} else {
		close(fd[1]);
		nbytes = read(fd[0],
			      &readbuffer,
			      sizeof(int));
		printf("P2 Received number: %d\n", readbuffer);
		int x = readbuffer;
		int y = x*2;

		// Mkfifo
		int named_fd;
		char *myfifo = "/tmp/myfifo";
		mkfifo(myfifo, 0666);

		printf("P2 Sending number %d to %s\n", y, myfifo);
		named_fd = open(myfifo, O_WRONLY);
		write(named_fd, &y, sizeof(int));
		close(named_fd);
		unlink(myfifo);
	}
	return (0);
}
