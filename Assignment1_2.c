/*
	Trapezoidal Rule using MPI
	Author: Carlos Galdamez 
	Modification of code from 'Introduction to Parallel Programming' by Peter S. Pacheco. 
	Code provided in book contained trapezoidal rule parallelized implementation but with 
	hardcoded values. This version takes in the values of n a and b and estimates the area
	under the curve based on the given parameters. For this example a function was created
	for testing. The function that is being tested on is f(x) = 2x + 1.
*/

#include <stdio.h>
#include <mpi.h>

// Functions used in main program
void getInput(int* n, double* a, double* b, int my_rank, int comm_sz);
double Trap(double left_endpt,double right_endpt,int trap_count,double base_len);
double f(double x);

int main(void){
	//int my_rank, comm_sz, n=1024, local_n;
	//double a = 0.0, b = 3.0, h, local_a, local_b;
	int my_rank, comm_sz, n, local_n;
	double a, b, h, local_a, local_b;
	double local_int, total_int;
	int source;

	MPI_Init(NULL,NULL);							// Initial setup
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);	
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	getInput(&n,&a,&b,my_rank,comm_sz);				// Get user input from user

	h = (b-a)/n;									// h is the width of the trapezoid
	local_n = n/comm_sz;							// local_n is the number of trapezoids each process will handle

	local_a = a + my_rank*local_n*h;				// a is the left most point of the trapezoid, local_a is a leftmost point for the set of trapezoids a process is taking care of
	local_b = local_a + local_n * h;				// b is the right most point of the trapezoid, local_b is a rightmost point for the set of trapezoids a process is taking care of
	local_int = Trap(local_a,local_b,local_n,h);	// returns sum of collection of trapazoid areas handled by the process


	// Alternative to problem 1. This uses collective communications.
	if(my_rank !=  0){
		MPI_Reduce(&local_int,&total_int,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	}
	else{
		MPI_Reduce(&local_int,&total_int,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	}
	
	//if(my_rank != 0){
	//	MPI_Send(&local_int,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);									// All processes send their result to process 0;
	//}
	//else{
	//	total_int = local_int;
	//	for (source = 1;source < comm_sz;source ++){
	//		MPI_Recv(&local_int,1,MPI_DOUBLE, source,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);		// Process 0 receives results from other process and adds it to the overall sum
	//		total_int += local_int;
	//	}
	//}

	if(my_rank == 0){																			// Only process 0 prints results
		printf("With n = %d trapezoids, out estimate \n", n);
		printf("of the integral from %f to %f = %.15e\n",a,b,total_int);
	}

	MPI_Finalize();																				// Release any resources being used by MPI program.
	return 0;
}

void getInput(
	int* n 		/*out*/,									
	double* a 	/*out*/,
	double* b 	/*out*/,
	int my_rank /*in*/,
	int comm_sz /*in*/){

	int dest;

	if(my_rank == 0){														// only Process 0 gets input from user
		printf("Print n, a, and b\n");
		scanf("%d %lf %lf", n,a,b);

		for(dest = 1; dest < comm_sz; dest++){								// Process 0 sends input to all other processes.
			MPI_Send(n,1,MPI_INT,dest,0,MPI_COMM_WORLD);
			MPI_Send(a,1,MPI_DOUBLE,dest,0,MPI_COMM_WORLD);
			MPI_Send(b,1,MPI_DOUBLE,dest,0,MPI_COMM_WORLD);
		}
	}
	else{
		MPI_Recv(n,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);			// Processes receive input from process 0
		MPI_Recv(a,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Recv(b,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
}


// Estimates area of a trapezoids between the left and right end points given
double Trap(
	double left_endpt	/*in*/,
	double right_endpt	/*in*/,
	int trap_count 		/*in*/,	
	double base_len		/*in*/){

	double estimate,x;
	int i;

	estimate = (f(left_endpt) + f(right_endpt))/2.0;		// Area of a trapezoid with multiplying the base length h
	for (i = 1; i <= trap_count-1; i ++){
		x = left_endpt + i*base_len;
		estimate += f(x);
	}

	estimate = estimate*base_len;							// Multiply are of all trapezoids by the base length

	return estimate;
}

// Simple function for testing
double f(double x){
	return 2 * x + 1;
}
