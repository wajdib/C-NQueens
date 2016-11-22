/*
 *  Author: Wajdi Bazuhair
 */

#include <stdio.h>
#include <stdlib.h>

 struct ti {
	int n;
	int first;
	int out_flag;
	int result;
} ;

struct ti *thread_info_t;

main(int argc, char *argv[]){
	if (argc !=2 ) {
		printf("Incorrect arguments, please provide 1 int argument");
		exit(1);
	}
	//store our input argument as int
	int n = atoi(argv[1]);
	
	printf("Assignment 4 program written by Wajdi Bazuhair \n");

	//generate_n_queens_serial(n, 0);
	int i = 0;
	thread_info_t = malloc(sizeof(struct ti));
	//assert(thread_info_t != NULL);
	thread_info_t->n = n;
	thread_info_t->out_flag = 1;
	for (i = 0; i < n; i++)
	{
		//generate_n_queens_serial_one(n, 1, i);
		thread_info_t->first = i;
		queens_thread(thread_info_t);
	}
	

}
