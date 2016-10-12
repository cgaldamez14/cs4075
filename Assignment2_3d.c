/*
   Sum of Prefix
   Author: Carlos Galdamez
   Description: Parallel program that finds prefix sums of a list of numbers. For this version
   we use MPI_Scan method to construct a list of prefix sums. 

   If we have the list : 1 2 3 4 , then the prefix sums list would be 1, 1+2, 1+2+3, 1+2+3+4.
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void generate_values();
void print_prefix_sums();

int *elements, *prefix_sums, *local_elements, *local_prefix_sums;
int my_rank, comm_sz;
int num_elements = 8;

int main ( void ){

	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	elements = malloc( num_elements * sizeof(int));
	prefix_sums = malloc( num_elements * sizeof(int));
	local_elements = malloc( (num_elements / comm_sz) * sizeof(int));
	local_prefix_sums = malloc( (num_elements / comm_sz) * sizeof(int));

	generate_values();

	print_prefix_sums();

	MPI_Finalize();
	return 0;
}

void generate_values(){

	if(my_rank == 0){
		srand(time(NULL));

		int random_number;

		for(int i = 0; i < num_elements; i++){
			random_number = rand() % 5 + 1;
			elements[i] = random_number;
		}
	}

	MPI_Scatter(elements, num_elements/ comm_sz, MPI_INT, local_elements, num_elements/ comm_sz, MPI_INT, 0, MPI_COMM_WORLD);

	int sum = 0;
	for (int i = 0; i < num_elements/comm_sz; i++){
		sum += local_elements[i];
		local_prefix_sums[i] = sum;
	}

	int *a = NULL;
	a = malloc(num_elements/comm_sz * sizeof(int));
	MPI_Scan(&local_prefix_sums[(num_elements/comm_sz) - 1],&a[(num_elements/comm_sz) - 1],1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);

	int *b = NULL;
	b = (num_elements/comm_sz * sizeof(int));
	for (int i = 0; i < num_elements/comm_sz; i++){
		sum += local_elements[i];
		local_prefix_sums[i] = sum;
	}
	
	printf("Prefix Sums: %d : ", my_rank);
	for (int i = 0; i < num_elements/comm_sz; i++){
		printf("%d ", a[i]);
	}
	printf("\n");
	free(a);
	free(b);
}

void print_prefix_sums(){

	if(my_rank == 0){
		printf("Original List of random integers: ");
		for (int i = 0; i < num_elements; i++){
			printf("%d ", elements[i]);
		}
		printf("\n");

		//printf("Prefix Sums: ");
		//for (int i = 0; i < num_elements; i++){
		//	printf("%d ", prefix_sums[i]);
		//}
		//printf("\n");
	}


}