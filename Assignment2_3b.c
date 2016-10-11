#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

void get_n (int* n, int my_rank);
void get_elements (int* elements, int* local_elements, int n, MPI_Datatype type, int my_rank);
void print_prefix_sum (int* prefixSums, int num );

int main (void){
	int number_of_elements, local_n;
	int my_rank, comm_size;
	int *elements, *local_elements, *prefix_sums;
	MPI_Datatype type;

	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	get_n(&number_of_elements, my_rank);

	local_n = number_of_elements / comm_size;

	MPI_Type_contiguous(local_n,MPI_INT, &type);
	MPI_Type_commit(&type);

	elements = malloc(number_of_elements * sizeof(int));
	prefix_sums = malloc(number_of_elements * sizeof(int));
	local_elements = malloc(local_n * sizeof(int));

	get_elements(elements, local_elements, number_of_elements, type, my_rank);

	//int sum = 0;
	//for (int i = 0; i < number_of_elements; i++){
	//	sum += elements[i];
	//	prefix_sums[i] = sum;
	//}

	//print_prefix_sum(prefix_sums,number_of_elements);

	free(prefix_sums);
	free(elements);

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

void print_prefix_sum (int* prefix_sums, int num ){

	printf("Prefix Sums: ");
	for (int i = 0; i < num; i++)
		printf("%d ",prefix_sums[i]);
	printf("\n");

}