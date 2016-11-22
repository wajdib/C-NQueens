/*
 *  Author: Wajdi Bazuhair
 */

#include <stdio.h>
#include  <string.h>

int generate_n_queens_serial(int n, int print_flag);
void get_array_string(int *list, int n, char *s);
int next_permutation(int *list, int n);
int check_diagonal(int *list, int n);
int generate_n_queens_serial_one(int n, int out_flag, int first);
void *queens_thread(void *infopointer);


typedef struct ti {
	int n;
	int first;
	int out_flag;
	int result;
} thread_info_t;





int generate_n_queens_serial(int n, int print_flag)
{
	int list[n];
	int p = 0;
	int solutions = 0;
	//fill the list with initial input
	int i = 0;
	for (i = 0; i < n; i++)
	{
		list[i] = i;
	}
	//n is the size of the input,  
	//4 * n is the number of spaces needed
	//+1 for the new line character
	char s[n + 4*n + 1];
	p++;

	//check the initial permutation for diagonal
	if (check_diagonal(list, n) == 1)
	{
		//satisfies it
		solutions++;


		if (print_flag == 1)
		{
			get_array_string(list, n, s);
			printf("%s", s);
		}
	}
	while (next_permutation(list, n) == 0)
	{
		p++;
		if (check_diagonal(list, n) == 1)
		{
			solutions++;
			if (print_flag == 1)
			{
				get_array_string(list, n, s);
				printf("%s", s);
			}
		}
	}

	printf("Total number of permutations = %d \n", p);
	printf("Total number of solutions = %d \n", solutions++);
}



int generate_n_queens_serial_one(int n, int out_flag, int first)
{
	int list[n];
	int p = 0;
	int solutions = 0;
	//fill the list with initial input
	int i = 0;
	for (i = 0; i < n; i++)
	{
		list[i] = i;
	}
	//n is the size of the input,  
	//4 * n is the number of spaces needed
	//+1 for the new line character
	char s[n + 4 * n + 1];
	p++;

	if (list[0] == first)
	{
		//check the initial permutation for diagonal
		if (check_diagonal(list, n) == 1)
		{
			//satisfies it
			solutions++;


			if (out_flag == 1)
			{
				get_array_string(list, n, s);
				printf("%s", s);
			}
		}
	}
	while (next_permutation(list, n) == 0)
	{
		p++;
		if (list[0] == first)
		{
			if (check_diagonal(list, n) == 1)
			{
				solutions++;
				if (out_flag == 1)
				{
					get_array_string(list, n, s);
					printf("%s", s);
				}
			}
		}
	}

	//printf("Total number of permutations = %d \n", p);
	//printf("Total number of solutions = %d \n", solutions++);

	return solutions;
}

void *queens_thread(void *infopointer)
{
	int result = generate_n_queens_serial_one(((struct ti*)infopointer)->n, ((struct ti*)infopointer)->out_flag, ((struct ti*)infopointer)->first);
	((struct ti*)infopointer)->result = result;

	printf("The value of first = %d and the number of solutions = %d \n", ((struct ti*)infopointer)->first, result);

	return NULL;
}

void get_array_string(int *list, int n, char *s)
{
	int i = 0;
	//initial null termination so that strlen in the first loop iteration does not print garbage
	s[0] = '\0';
	for (i = 0; i < n; i++)
		//each time add strlen(s) so append the new values
		sprintf(s  + strlen(s), "%d    ", *(list+i));
	//append the new line character
	sprintf(s + strlen(s), "%c", '\n');

}

int next_permutation(int *list, int n)
{
		int i, j;
		int temp;

		i = n - 1;
		while (i > 0 && list[i - 1] >= list[i])
			i--;
		if (i == 0)
			//already at the last permutation
			return 1;

		j = n - 1;
		while (list[j] <= list[i - 1])
			j--;
		temp = list[i - 1];
		list[i - 1] = list[j];
		list[j] = temp;

		j = n - 1;
		while (i < j) {
			temp = list[i];
			list[i] = list[j];
			list[j] = temp;
			i++;
			j--;
		}
		return 0;
}

int check_diagonal(int *list, int n)
{
	int i = 0;
	int j = 0;
	int colDiff = 0;
	int rowDiff = 0;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			// Don't compare the same indexes, will always return false
			if (i == j )
				continue;
			

			colDiff = abs((j+1) - (i+1)); 
			rowDiff = abs(*(list + j) - *(list + i));

			if (colDiff == rowDiff)
			{
				//same diagonal
				return 0;
			}
		}
	}
	// reached the end, it means it satisfies the diagonal property
	return 1;
}
