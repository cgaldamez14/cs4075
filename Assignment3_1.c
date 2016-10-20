/*
	Pi Estimation Using Monte Carlo
	Description: Program uses Monte Carlo method to estimate the value of PI
	Author: Carlos Galdamez

	Compile: mpicc -g -Wall -o monte_carlo Assignment3_1.c 
	Exexute:  mpiexec -n 4 monte_carlo

	Number of tosses tested: 262144
	Answer: 3.140747
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void read_toss_number();
void monte_carlo();
void display_pi();

int comm_sz, my_rank;
long long int tosses,local_tosses, number_in_circle;

int main(void){

	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	read_toss_number();

	local_tosses = tosses / comm_sz;

	monte_carlo();

	display_pi();

	MPI_Finalize();
	return 0;
}

// Read how manu tosses and broadcast
void read_toss_number(){

	if(my_rank == 0){
		printf("How many tosses?\n");
		scanf("%llu",&tosses);
	}

	MPI_Bcast(&tosses,1,MPI_LONG_LONG,0,MPI_COMM_WORLD);
}

// Pseudo code provided by Zhu
void monte_carlo(){

	long long int number_in_circle_local = 0;
	double x;
	double y;
	double distance_squared;

	for (long long int toss = 0; toss < local_tosses; toss++){
		x = (((double)rand() / (double)RAND_MAX) * (2.0)) - 1.0;			// Random number between -1 and 1
		y = (((double)rand() / (double)RAND_MAX) * (2.0)) - 1.0;			// Random number between -1 and 1 
		distance_squared = x * x + y * y;
		if (distance_squared <= 1) number_in_circle_local++;
	}

	MPI_Reduce(&number_in_circle_local,&number_in_circle,1,MPI_LONG_LONG,MPI_SUM,0,MPI_COMM_WORLD);  // Reduce all number_in_circle_local
}

// Display pi estimation
void display_pi(){

	// Only process 0 does the rest of the estimate
	if (my_rank == 0){
		double pi_estimate = 4 * number_in_circle / ((double) tosses);
		printf("Pi Estimate: %lf\n", pi_estimate);
	}
}