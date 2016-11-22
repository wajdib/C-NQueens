/*
 *  Author: Wajdi Bazuhair
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "restart.h"
#include "uici.h"
#include <pthread.h>
#define ENOUGH ((CHAR_BIT * sizeof(int) - 1) / 3 + 2)

int copy2files(int fromfd1, int tofd1, int fromfd2, int tofd2);

#define BLKSIZE 1024
//functions from assignment 4////////////////////////

int generate_n_queens_serial(int n, int print_flag);
void get_array_string(int *list, int n, char *s);
int next_permutation(int *list, int n);
int check_diagonal(int *list, int n);
int generate_n_queens_server_one(int n, int first, int communfd);
void *queens_thread(void *infopointer);

int SendStringToClient(char* data, int tofd, int dataSize);


static pthread_mutex_t  mutexLock = PTHREAD_MUTEX_INITIALIZER;
int totalSolutions = 0;
struct ti {
	int n;
	int first;
	int result;
	int communfd;
};

struct ti *thread_info_t;

void *queens_thread(void *infopointer)
{
	int result = generate_n_queens_server_one(((struct ti*)infopointer)->n, ((struct ti*)infopointer)->first, ((struct ti*)infopointer)->communfd);
	((struct ti*)infopointer)->result = result;

	pthread_mutex_lock(&mutexLock);

	totalSolutions = totalSolutions + result;
	pthread_mutex_unlock(&mutexLock);

	char strSolution[ENOUGH + 25];
	sprintf(strSolution, "Total Solutions = %d /DONE", totalSolutions);
	int SolbufferSize = sizeof(strSolution);
	SendStringToClient(strSolution, ((struct ti*)infopointer)->communfd, SolbufferSize);

       fprintf(stderr, "Thread done executing\n");
	return NULL;
}



/////////////////////////////////////////////////////
// Read received string from client, string consists of n_value and first_value
int ReadData(int fromfd, char* ReceivedData) {

	int bytesread, byteswritten;
	int totalbytes = 0;


	if ((bytesread = r_read(fromfd, ReceivedData, BLKSIZE)) <= 0)
	{
		return -1;
	}
	totalbytes += bytesread;

	return totalbytes;

}

/*
 *                       UICI Server
 *  Open a UICI port specified as a command-line argument
 *  and wait for requests.  When a request arrives,
 *  fork a child to handle the communication and resume
 *  waiting for another request.
 */
int main(int argc, char *argv[])
{
	pthread_t *tids;
        int threadID = 0;




	u_port_t portnumber;
	int listenfd;
	int communfd;
	char client[MAX_CANON];
	int bytes_copied = 0;
	pid_t child;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		return 1;
	}

	portnumber = (u_port_t)atoi(argv[1]);
	if ((listenfd = u_open(portnumber)) < 0) {
		perror("Listen endpoint creation failed");
		return 1;
	}

	fprintf(stderr, "[%ld]: Waiting for the first connection on port %d\n",
		(long)getpid(), (int)portnumber);
	for (;;) {
		if ((communfd = u_accept(listenfd, client, MAX_CANON)) != -1) {
			fprintf(stderr, "[%ld]: A connection has been received from %s\n",
				(long)getpid(), client);
			if ((child = fork()) == -1)
				perror("Could not fork a child");

			if (child == 0) {                            /* child code */
				r_close(listenfd);
				//Send /READY flag to client to let it know that the forking is done
				char ready[] = "/READY";
				int Readysize = sizeof(ready);
				SendStringToClient(ready, communfd, Readysize);
				fprintf(stderr, "Sent Ready flag to client\n");
				int Child_bytes_copied = 0;

				for (;;)
				{
					
					char BufferReceived[BLKSIZE];
					Child_bytes_copied =
						ReadData(communfd, BufferReceived);


					fprintf(stderr, "Finished reading the buffer !\n");
					fprintf(stderr, "received %s!\n", BufferReceived);
					//execute generate_n_queens_serial_one
					//extract the required integer values from the received data
					int n_value = 0;
					int first_value = 0;
					int n = sscanf(BufferReceived, "%d %d /END", &n_value, &first_value);
					if (n != 2)
					{
						fprintf(stderr, "Problem occured when extracting integer values from the received buffer!\n");
						return 0;
					}


					//assign a thread for this job
					tids = (pthread_t *)malloc(sizeof(pthread_t *));
					if (tids == NULL) {
						fprintf(stderr, "Error allocating space for thread ids\n");
						return 1;
					}
					threadID++;
					thread_info_t = malloc(sizeof(struct ti));
					thread_info_t->first = first_value;
					thread_info_t->n = n_value;
					thread_info_t->communfd = communfd;
					if (pthread_create(tids+threadID, NULL, queens_thread, thread_info_t)) {
						fprintf(stderr, "Error creating thread \n");
						return 1;
					}
					

				}

				r_close(communfd);
				return 0;
			}
			else {                                    /* parent code */
				r_close(communfd);
				while (r_waitpid(-1, NULL, WNOHANG) > 0);  /* clean up zombies */
			}
		}
		else
			perror("Accept failed");
	}
}





