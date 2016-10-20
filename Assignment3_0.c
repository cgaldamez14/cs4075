/*
	Matrix-Vector Multiplication
	Description: User enters a matric and vector. Program will calculate and display the result for the multiplication between the matrix in vector.
				 It is assumed column number and vector order are the same. And that the number of element in the array is evenly divisible by 'comm'sz'
	Author: Carlos Galdamez

	Compile: mpicc -g -Wall -o matrix_mult Assignment3_0.c
	Exexute: mpiexec -n 2 matrix_mult
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void error_check();
void read_order();
void read_input();
void matrix_vect_mult();
void print_vect();

int comm_sz, my_rank;
int n, local_n, matrix_rows, matrix_columns;
int *matrix, *local_block, *vector, *result;

int main( void ){

	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	read_order();

	local_n = ((matrix_rows * matrix_columns) / comm_sz);

	// Allocate memory for arrays that will be used
	matrix = malloc(matrix_rows * matrix_columns * sizeof(int));
	local_block = malloc(local_n * sizeof(int));
	vector = malloc(n * sizeof(int)); 
	result = malloc(matrix_rows * sizeof(int));

	read_input();
	matrix_vect_mult();
	print_vect();

	// Release any resources being used by MPI
	MPI_Finalize();

	free(matrix);
	free(local_block);
	free(vector);
	free(result);

	return 0;
}

// Checks for error but for some reason if this is included the program does not work as expected
void error_check(){
	int error = 0;
	if(my_rank == 0){
		if(matrix_columns < n){
			printf("The number of columns should be the same as the order of the vector\n");
			error = 1;
		}
		if(matrix_rows % comm_sz != 0){
			printf("The number of rows in the matrix should be evenly divisible by 'comm_sz'\n");
			error = 1;
		}
	}

	MPI_Bcast(&error,1,MPI_INT,0,MPI_COMM_WORLD);

	if (error == 1){
		MPI_Finalize();
		exit(-1);
	}

}

// Read order and matrix size from user
void read_order(){

	if(my_rank == 0){
		printf("What is the order of the vector?\n");
		scanf("%d",&n);

		printf("How many columns does your matrix consists of?\n");
		scanf("%d",&matrix_columns);

		printf("How many rows does your matrix consists of?\n");
		scanf("%d",&matrix_rows);

		//error_check();
	}

	MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&matrix_columns,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&matrix_rows,1,MPI_INT,0,MPI_COMM_WORLD);


}

// Read in vector and matrix values
void read_input(){

	if(my_rank == 0){
		printf("Enter the %d x %d matrix: \n",matrix_rows,matrix_columns);
		for(int i = 0; i < matrix_rows * matrix_columns; i++)
			scanf("%d", &matrix[i]);
	}

	MPI_Scatter(matrix,local_n,MPI_INT,local_block,local_n,MPI_INT,0,MPI_COMM_WORLD); // Scatter blocks from array to processes

	if(my_rank == 0){
		printf("Enter the vector: \n");
		for(int i = 0; i < n; i++)
			scanf("%d", &vector[i]);
	}

	MPI_Bcast(vector,n,MPI_INT,0,MPI_COMM_WORLD); // Scatter whole vector to all processes
}

// Function used by every process to calculate the matrix vector multiplications
void matrix_vect_mult(){

	int local_mult[(local_n / matrix_columns)];      // Should only be on column with n rows

	// Initialize vector to all 0
	for (int i =0; i < (local_n / matrix_columns);i++)
		local_mult[i] = 0;
	
	// Does vector multiplication
	for (int i = 0; i < local_n; i++){
		local_mult[ i/n ] += (local_block[i] * vector[ i % n ]); 
	}

	// Process 0 will gather all results from all processes
	MPI_Gather(local_mult,local_n/matrix_columns,MPI_INT,result,local_n/matrix_columns,MPI_INT,0,MPI_COMM_WORLD);
}

// Function to print resulting vector
void print_vect(){

	if(my_rank == 0){
		printf("Matrix Vector Multiplication Result: \n");
		for(int i = 0; i < matrix_rows; i++){
			printf("%d\n", result[i]);
		}
	}
}


