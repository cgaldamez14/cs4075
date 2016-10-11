#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

void read_vectors(int* v1, int* v2, int* local_v1, int* local_v2, int order, int local_order, MPI_Datatype type, int my_rank, int comm_sz);
void read_n(int *order, int *local_order, int my_rank, int comm_sz);
void print_vector(int* vector ,int order, int my_rank);
void vec_sum(int* local_v1, int* local_v2, int* local_sum, int local_order);

int main(void){
	int my_rank, comm_sz;
	int order, local_order;
	int *vec1, *vec2, *local_vec1, *local_vec2;
	int *sum, *local_sum;

	MPI_Datatype type;

	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	// Read in vector order
	read_n(&order, &local_order, my_rank, comm_sz);

	vec1 = malloc(order * sizeof(int));
	vec2 = malloc(order * sizeof(int));
	local_vec1 = malloc(local_order * sizeof(int));
	local_vec2 = malloc(local_order * sizeof(int));
	sum = malloc(order * sizeof(int));
	local_sum = malloc(local_order * sizeof(int));


	MPI_Type_contiguous(local_order,MPI_INT, &type);
	MPI_Type_commit(&type);

	// Read in vectors
	read_vectors(vec1, vec2, local_vec1, local_vec2, order, local_order, type, my_rank, comm_sz);

	vec_sum(local_vec1, local_vec2, local_sum, local_order);

	MPI_Gather(local_sum, 1, type, sum, 1, type, 0, MPI_COMM_WORLD);

	print_vector(sum, order, my_rank);

	free(vec1);
	free(vec2);
	free(local_vec1);
	free(local_vec2);
	free(sum);
	free(local_sum);

	MPI_Finalize();
	return 0;
}

void read_n(int* order, int* local_order, int my_rank, int comm_sz){

	if(my_rank == 0){
		printf("What is the order of the vectors?\n");
		scanf("%d",order);
	}

	MPI_Bcast(order,1,MPI_INT,0,MPI_COMM_WORLD);

	*local_order = *order / comm_sz;
}

void read_vectors(int* v1, int* v2, int* local_v1, int* local_v2, int order, int local_order,MPI_Datatype type, int my_rank, int comm_sz){

	if(my_rank == 0){
		printf("Enter %d integers for vector 1:\n", order);
		for (int i = 0; i < order; i++)
			scanf("%d",&v1[i]);

		printf("Enter %d integers for vector 2:\n", order);
		for (int i = 0; i < order; i++)
			scanf("%d",&v2[i]);
	}

	MPI_Scatter(v1,1, type, local_v1, 1, type, 0, MPI_COMM_WORLD);
	MPI_Scatter(v2,1, type, local_v2, 1, type, 0, MPI_COMM_WORLD);

}

void print_vector(int* vector ,int order, int my_rank){
	if (my_rank == 0){
		for (int i = 0; i < order; i++){
			printf("%d ", vector[i]);
		}
	}
}

void vec_sum(int* local_v1, int* local_v2, int* local_sum, int local_order){

	for (int i = 0; i < local_order; i++)
		local_sum[i] = local_v1[i] + local_v2[i];

}

