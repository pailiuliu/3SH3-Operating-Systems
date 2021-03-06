
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define P2_READ     0			// Parent reads from this node 
#define CH_WRITE    1			// Child writes on this node 
#define CH_READ     2			// Child reads from this node 
#define P2_WRITE    3			// Parent writes to this node 

int main(int argc, char *argv[])
{
	int fd[4];      
	int input = 0;
	int result = 0, len;
	pid_t pid;

	if(pipe(fd) | pipe(fd+2)) {
		
		perror("Pipes couldnt be located");
		exit(EXIT_FAILURE); // same as exit(1) but prevents warnings
	}

	if ((pid = fork()) < 0) {
		perror("Failed to create child process");
		return EXIT_FAILURE; 
	}

	// Child process initialization
	if (pid == 0) {
		close(fd[P2_READ]);      // close parent process
		close(fd[P2_WRITE]);

		pid = getpid();

		while(input != -1) {
			printf("Child(%d): Enter a numerical value to send to parent process: ", pid);
			scanf("%d", &input);
		
			// write() returns numbner of bytes written 
			if (write(fd[CH_WRITE], &input, sizeof(input)) != sizeof(input)) {		
				perror("Child: COuldn't send the data ");
				exit(EXIT_FAILURE);
			}
		}

		/* parent sends value back and read it thru CH READ */
		len = read(fd[CH_READ], &result, sizeof(result));
		
		if (len < 0) {
			perror("No data found");
			exit(EXIT_FAILURE);
		} else {
			printf("Child(%d): Total Sum : %d\n", pid, result);
		}

		close(fd[CH_READ]);		
		close(fd[CH_WRITE]);

		return 0;
	}
	
	// Parent process initialization
	close(fd[CH_READ]);	// close child process			
	close(fd[CH_WRITE]);

	pid = getpid();

	/* child sends value and read it thru P2 READ and store it */
	while(input != -1) {
		len = read(fd[P2_READ], &input, sizeof(input));

		if (len < 0) {
			perror("No data found");
			exit(EXIT_FAILURE);
		} else {
			
			result += input; // summ all inputs
		}

	}
	result++;
	
	
	if (write(fd[P2_WRITE], &result, sizeof(result)) < 0) {
		perror("Parent: Failed to write response value");
		exit(EXIT_FAILURE);
	}

	close(fd[P2_READ]);		
	close(fd[P2_WRITE]);

	// Wait for child termination and then terminate 
	wait(NULL);
	return 0;
}
