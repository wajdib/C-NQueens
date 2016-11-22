/*
 *  Author: Wajdi Bazuhair
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "uici.h"
#include "restart.h"
#include <pthread.h>
int copy2files(int fromfd1, int tofd1, int fromfd2, int tofd2);


static pthread_mutex_t  mutexLock = PTHREAD_MUTEX_INITIALIZER;
// Sends a char array as a string to server, the string will contain integer n and integer first seperated by a space
#define BLKSIZE 1024
int SendStringToServer(char* data, int tofd, int dataSize) {
	int i = 0;
	char buf[dataSize];
	int bytesread, byteswritten;
	int totalbytes = 0;


	if ((byteswritten = r_write(tofd, data, dataSize)) == -1)
		return -1;
	totalbytes += byteswritten;

	return totalbytes;
}
////////////////////////////////////////////////////////////////
// Read received string from client, string consists of n_value and first_value
int ReadResultDataFromServer(int fromfd, char* ReceivedData) {
	int bytesread, byteswritten;
	int totalbytes = 0;

	if ((bytesread = r_read(fromfd, ReceivedData, BLKSIZE)) <= 0)
		return -1;
	totalbytes += bytesread;

	return totalbytes;
}
///////////////////////////////////////////////////////////////////

struct ti {
	char* host;
	int portNumber;
	
};

struct ti *thread_info_t;

void *Client_thread(void *infopointer)
{

	   int communfdH;
                if ((communfdH = u_connect(((struct ti*)infopointer)->host)->portNumber, ((struct ti*)infopointer)->host)) == -1) {
		perror("Failed to establish connection");
		return NULL;

		fprintf(stderr, "[%ld]:connection made to %s\n", (long)getpid(), argv[1]);

	//wait for the ready flag from server before transmitting anything
	for (;;)
	{
		char BufferReceived[BLKSIZE];
		ReadResultDataFromServer(communfd, BufferReceived);
		char* s;
		s = strstr(BufferReceived, "/READY");
		if (s != NULL)
		{
			//server is now ready
			//fprintf(stderr, "Received /READY flag from server \n");
			break;
		}

	}

	int i = 0;
	for (i = 0; i <= n_value; i++)
	{
		//send the n requests to the server
		//convert our int data to string and store it in char string in this format: 'n_value first_value /END'
		first_value = i;
		char str[n_value + first_value + 5];
		str[0] = '\0';
		sprintf(str, "%d ", n_value);
		sprintf(str + strlen(str), "%d", first_value);
		sprintf(str + strlen(str), " /END");
		int bufferSize = sizeof(str);
		//now our str contains both n value and first value seprated by a space character


		//send string data to server
		bytescopied = SendStringToServer(str, communfd, bufferSize);//copy2files(communfd, STDOUT_FILENO, STDIN_FILENO, communfd);
		//fprintf(stderr, "[Client] : sent buffer size = %d. String data = %s (first integer is n, second is First) to server\n", bufferSize, str);
	}

	//wait for the results to come in from the server
	int totalbytesReceived = 0;
	for (;;)
	{
		char BufferReceived[BLKSIZE];
		char *s;
		totalbytesReceived = ReadResultDataFromServer(communfd, BufferReceived);

		if (totalbytesReceived != -1)
		{
			s = strstr(BufferReceived, "/DONE");
			if (s != NULL)
			{
				//remove the done flag before outputing the results
				strncpy(s, "", 5);
			}
  			pthread_mutex_lock(&mutexLock);
			fprintf(stderr, "%s \n", BufferReceived);
 			pthread_mutex_unlock(&mutexLock);
		}
		else if (/*s!= NULL &&*/ totalbytesReceived == -1)
			break;
	}
	pthread_mutex_lock(&mutexLock);
	fprintf(stderr, "[%ld]:transferred %d bytes\n", (long)getpid(), bytescopied);
	pthread_mutex_unlock(&mutexLock);
	

	
	return NULL;
}


int main(int argc, char *argv[]) {

pthread_t *tids;
        int threadID = 0;
	int bytescopied;
	int communfd;
	u_port_t portnumber;
	int n_value, first_value;

	if (argc != 4) {
		fprintf(stderr, "Usage: %s n_value port host \n", argv[0]);
		return 1;
	}
	n_value = atoi(argv[1]);

	portnumber = (u_port_t)atoi(argv[2]);
	char* textName ;
	textName =  argv[3];
       
	FILE * fp;
       char * line = NULL;
       size_t len = 0;
       ssize_t read;

       fp = fopen(textName, "r");
       if (fp == NULL)
           exit(EXIT_FAILURE);

       while ((read = getline(&line, &len, fp)) != -1) {
           //printf("%s", line);

					tids = (pthread_t *)malloc(sizeof(pthread_t *));
					if (tids == NULL) {
						fprintf(stderr, "Error allocating space for thread ids\n");
						return 1;
					}
					threadID++;
					thread_info_t = malloc(sizeof(struct ti));
					thread_info_t->host = line;
  					thread_info_t->portnumber = portnumber;
					if (pthread_create(tids+threadID, NULL, client_thread, thread_info_t)) {
						fprintf(stderr, "Error creating thread \n");
						return 1;
					}
            
	}
       }


	
	

	return 0;
}
