/*
	Vector Addition
	Author: Carlos Galdamez
	Description: Program asks user to enter the order of the vectors. The order of the vectors
				 should be divisible by 'comm_sz'. The program then asks for vector one then
				 vector two. The program will display the original two vectors and the result
				 of vector addition. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

void print_vector(int* vector ,int order, int my_rank);
void read_n(int *order, int *local_order, int my_rank, int comm_sz);
void vec_sum(int* local_v1, int* local_v2, int* local_sum, int local_order);
void read_vectors(int* v1, int* v2, int* local_v1, int* local_v2, int order, int local_order, int my_rank, int comm_sz);

int main(void){
	int my_rank, comm_sz;
	int order, local_order;
	int *vec1, *vec2, *local_vec1, *local_vec2;
	int *sum, *local_sum;

	// Initialize communication between processes
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	// Read in vector order
	read_n(&order, &local_order, my_rank, comm_sz);

	// Allocate memory for list that will be used
	vec1 = malloc(order * sizeof(int));
	vec2 = malloc(order * sizeof(int));
	local_vec1 = malloc(local_order * sizeof(int));
	local_vec2 = malloc(local_order * sizeof(int));
	sum = malloc(order * sizeof(int));
	local_sum = malloc(local_order * sizeof(int));

	// Read in vectors
	read_vectors(vec1, vec2, local_vec1, local_vec2, order, local_order, my_rank, comm_sz);

	// Have each process do vector addition for their local list
	vec_sum(local_vec1, local_vec2, local_sum, local_order);

	// Gather all parts of local sum and save in final 'sum' list
	MPI_Gather(local_sum, local_order, MPI_INT, sum, local_order, MPI_INT, 0, MPI_COMM_WORLD);

	if(my_rank == 0) printf("\nVector 1:\n");
	print_vector(vec1, order,my_rank);
	if(my_rank == 0) printf("Vector 2:\n");
	print_vector(vec2, order,my_rank);

	// Display answer
	if(my_rank == 0) printf("Vector Sum:\n");
	print_vector(sum, order, my_rank);

	// Free up allocated memory space
	free(vec1);
	free(vec2);
	free(local_vec1);
	free(local_vec2);
	free(sum);
	free(local_sum);

	// Free all reasources used for MPI programs
	MPI_Finalize();
	return 0;
}

void read_n(int* order, int* local_order, int my_rank, int comm_sz){

	// Only process 0 gets input from user
	if(my_rank == 0){
		printf("What is the order of the vectors?\n");
		scanf("%d",order);
	}

	// Broadcast input from user to other processes
	MPI_Bcast(order,1,MPI_INT,0,MPI_COMM_WORLD);

	*local_order = *order / comm_sz;
}


void read_vectors(int* v1, int* v2, int* local_v1, int* local_v2, int order, int local_order, int my_rank, int comm_sz){

	// Only process 0 gets input from user
	if(my_rank == 0){
		printf("Enter %d integers for vector 1:\n", order);
		for (int i = 0; i < order; i++)
			scanf("%d",&v1[i]);

		printf("Enter %d integers for vector 2:\n", order);
		for (int i = 0; i < order; i++)
			scanf("%d",&v2[i]);
	}

	// Split up two vectors between all the processes evenly
	MPI_Scatter(v1,local_order,MPI_INT, local_v1, local_order, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(v2,local_order,MPI_INT, local_v2, local_order, MPI_INT, 0, MPI_COMM_WORLD);

}

// Displays vector
void print_vector(int* vector ,int order, int my_rank){
	
	if (my_rank == 0){
		for (int i = 0; i < order; i++){
			printf("%d ", vector[i]);
		}
		printf("\n");
	}
}

// Calculates vector sum for two local vectors
void vec_sum(int* local_v1, int* local_v2, int* local_sum, int local_order){

	for (int i = 0; i < local_order; i++)
		local_sum[i] = local_v1[i] + local_v2[i];

}

