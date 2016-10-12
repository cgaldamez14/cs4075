/*
   Sum of Prefix
   Author: Carlos Galdamez
   Description: Parallel program that finds prefix sums of a list of numbers. For example:

   If we have the list : 1 2 3 4 , then the prefix sums list would be 1, 1+2, 1+2+3, 1+2+3+4.
*/

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

void get_n (int* n, int my_rank);
void print_prefix_sum (int* prefixSums, int num , int my_rank);
void get_elements (int* elements, int* local_elements, int n, MPI_Datatype type, int my_rank);
void get_prefix_sums (int* local_elements, int* local_sums, int local_n, int my_rank, int comm_size);

int main (void){
	int number_of_elements, local_n;
	int my_rank, comm_size;
	int *elements, *local_elements, *prefix_sums, *local_sums;
	MPI_Datatype type;

	// Initialize communication world
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	get_n(&number_of_elements, my_rank);

	local_n = number_of_elements / comm_size;

	// Create MPI Datatype
	MPI_Type_contiguous(local_n,MPI_INT, &type);
	MPI_Type_commit(&type);

	// Allocate memory
	elements = malloc(number_of_elements * sizeof(int));
	prefix_sums = malloc(number_of_elements * sizeof(int));
	local_elements = malloc(local_n * sizeof(int));
	local_sums = malloc(local_n * sizeof(int));

	get_elements(elements, local_elements, number_of_elements, type, my_rank);

	get_prefix_sums(local_elements, local_sums, local_n, my_rank, comm_size);

	// Gather all local lists
	MPI_Gather(local_sums, 1, type, prefix_sums, 1, type, 0, MPI_COMM_WORLD);

	print_prefix_sum(prefix_sums,number_of_elements,my_rank);

	// Free memory allocation of lists
	free(prefix_sums);
	free(elements);
	free(local_elements);
	free(local_sums);

	// Free resources used in MPI
	MPI_Finalize();
	return 0;
}

void get_n (int* n, int my_rank){

	if (my_rank == 0){
		printf("How many element do you want to get the prefix sums for?\n");
		scanf("%d",n);
	}

	MPI_Bcast(n, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

void get_elements (int* elements, int* local_elements, int n, MPI_Datatype type, int my_rank){

	if (my_rank == 0){
		printf("Enter %d integers:\n", n);
		for (int i = 0; i < n; i++)
			scanf("%d", &elements[i]);
	}

	MPI_Scatter(elements, 1, type, local_elements, 1, type, 0, MPI_COMM_WORLD);
}

void print_prefix_sum (int* prefix_sums, int num, int my_rank ){

	if(my_rank == 0){
		printf("Prefix Sums: ");
		for (int i = 0; i < num; i++)
			printf("%d ",prefix_sums[i]);
		printf("\n");
	}
}

// Sets prefix sums for local list
void get_prefix_sums (int* local_elements, int* local_sums, int local_n, int my_rank, int comm_size){
	int sum = 0;
	for (int i = 0; i < local_n; i++){
		sum += local_elements[i];
		local_sums[i] = sum;
	}

	int last_element;

	// Receive is done first because this ensures that next process is blocked until the first process finishes doing its prefix sums
	if (my_rank > 0 && my_rank <= comm_size - 1){
		MPI_Recv(&last_element, 1,MPI_INT, my_rank-1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for(int i = 0; i < local_n; i++)
			local_sums[i] = local_sums[i] + last_element;
	}

	if (my_rank >= 0 && my_rank < comm_size - 1){
		last_element = local_sums[local_n - 1];
		MPI_Send(&last_element, 1,MPI_INT, my_rank+1, 0,MPI_COMM_WORLD);
	}
}