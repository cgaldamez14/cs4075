/*
	Histogram using MPI
	Author: Carlos Galdamez 
	This program creates a histogram for a collection of float values entered
	by the user. The number of floats that will be enetered have to be specified before hand
	and need to be evenly divisible by the number of processes running the program.

	Testing data set: 5.6 3.2 1.6 7.6 10.5 3.3 6.6 3.5
	Compile: mpicc -o histogram Assignment1_3.c -lm     <--- The '-lm' is important in order to be able to use the math header file
	Execute: mpiexec -n 4 ./histogram


	Result:

	1 <= x < 2	 |X
	2 <= x < 3	 |
	3 <= x < 4	 |XXX
	4 <= x < 5	 |
	5 <= x < 6	 |X
	6 <= x < 7	 |X
	7 <= x < 8	 |X
	8 <= x < 9	 |
	9 <= x < 10	 |
	10 <= x < 11 |X

*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>  // Needed to use ceil and floor functions, -lm parameter needs to be added to compile


// Functions that will be used in the program
void initialize_count(int* local_total, int local_n);
void readN(int* n, int* local_n, int my_rank, int comm_sz);
void printLocalList(double* local_list, int local_n,int my_rank);
void count(int* local_total,double* local_list, int* min, int local_n, int my_rank, int comm_sz);
void printHistogram(int* local_total, int local_n, int bin_count, int* min, int my_rank, int comm_sz);
void readList(double* list, double* local_list, int* min, int* max,int* bin_count, int n, int local_n, int my_rank, int comm_sz);


int main( void ){
	
	int my_rank,comm_sz;
	int n,local_n;
	int min,max;
	double *list, *local_list;
	int bin_count;

	MPI_Init(NULL,NULL);															// Initial setup
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);										
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	readN(&n,&local_n,my_rank,comm_sz);												// Get n value from user, n value is the number of numbers they will enter

	list = malloc(n * sizeof(double));												// Allocate memory for the array list that will hold all the values
	local_list = malloc(local_n * sizeof(double));									// Allocate memory for local list of floats that each process will handle

	readList(list,local_list,&min,&max, &bin_count, n,local_n,my_rank,comm_sz);		// Read in list of floating point numbers from user

	free(list);																		// List is never used again, free resources used by list

	int local_total[bin_count];														// Array keeping count of how many numbers in the processes' local list fall within the same range

	initialize_count(local_total,bin_count);										// Initialize local totals to have all 0
	count(local_total,local_list,&min,local_n,my_rank,comm_sz);

	printHistogram(local_total,local_n,bin_count,&min,my_rank,comm_sz);				// Have process 0 gather all the results and display them
	
	free(local_list);																 
	MPI_Finalize();																	// Release all resources being used by MPI
	return 0;
}

// Used to initialize values of array to 0
void initialize_count( int* local_total /*in*/, 
					   int bin_count 	/*in*/){

	for(int i = 0; i < bin_count;i++){
		local_total[i] = 0;
	}
}

// Used to read from user, how many floating point number they will enter
void readN( int* n 			/*out*/, 
			int* local_n	/*out*/, 
			int my_rank		/*in*/, 
			int comm_sz 	/*in*/){

	if(my_rank == 0){
		printf("How many elements are being represented by the histogram?\n");
		scanf("%d", n);
	}

	MPI_Bcast(n,1,MPI_INT,0,MPI_COMM_WORLD);										// Broadcast value of n to all processes

	*local_n = *n/comm_sz;															// At this point all process have the value of n so there is no point in calculating local_n and
																					// broadcasting. Each process can figure out local_n by itself.

}

// Function will provide each process with a list to work with and broadcast min, max , and bin size to all processes.
void readList( double* list 		/*out*/,
			   double* local_list	/*out*/, 
			   int* min 			/*out*/, 
			   int* max				/*out*/, 
			   int* bin_count		/*out*/, 
			   int n 				/*in*/, 
			   int local_n			/*in*/, 
			   int my_rank			/*in*/, 
			   int comm_sz 			/*in*/ ){

	if(my_rank == 0){
		double mi = INFINITY;
		double ma = -INFINITY;
		printf("Enter %d floating point numbers to create a histogram\n", n);
		for(int i = 0; i < n; i++){
			scanf("%lf",&list[i]);
			if(list[i] < mi) mi = list[i];
			if(list[i] > ma) ma = list[i];
		}
		*min = (int)mi;
		*max = (int)(ma + 1);
		*bin_count = *max - *min;

		// Split array between all the processes
		MPI_Scatter(list,local_n,MPI_DOUBLE,local_list,local_n,MPI_DOUBLE,0,MPI_COMM_WORLD);

		// Broadcast values for min,max and bin size
		MPI_Bcast(min,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(max,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(bin_count,1,MPI_INT,0,MPI_COMM_WORLD);

	}
	else{
		MPI_Scatter(list,local_n,MPI_DOUBLE,local_list,local_n,MPI_DOUBLE,0,MPI_COMM_WORLD);

		MPI_Bcast(min,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(max,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(bin_count,1,MPI_INT,0,MPI_COMM_WORLD);

	}
}

// Used for testing, not used in final implementation
void printLocalList( double* local_list		/*in*/, 
					 int local_n			/*in*/,
					 int my_rank			/*in*/ ){
	for (int i = 0; i < local_n; i++){
		printf("my rank: %d, and my list: %lf ",my_rank,local_list[i]);
	}
	printf("\n");
}

// Counts numbers between certain values and updates count on local_total array
void count( int* local_total		/*out*/,
		    double* local_list		/*in*/, 
		    int* min 				/*in*/, 
		    int local_n 			/*in*/, 
		    int my_rank 			/*in*/, 
		    int comm_sz 			/*in*/){
	for(int i = 0; i < local_n;i++ ){
		int index = floor(local_list[i] - *min);			// Map values to indexes of the array by subtracting value from min and flooring that value
		local_total[index] += 1;
	}
}

// Displays histogram
void printHistogram( int* local_total	/*in*/, 
					 int local_n		/*in*/, 
					 int bin_count		/*in*/, 
					 int* min 			/*in*/, 
					 int my_rank		/*in*/, 
					 int comm_sz 		/*in*/){
	int* a = NULL;
   	int i;
   
   	if (my_rank == 0) {																			// Only process 0 prints
      	a = malloc(bin_count * comm_sz * sizeof(int));
    	MPI_Gather(local_total,bin_count,MPI_INT,a,bin_count,MPI_INT,0,MPI_COMM_WORLD);			// Gather function will receive all arrays from all the processes and put them together into one array
      	for (i=0;i<bin_count;i++){
      		printf("%d <= x < %d\t|", (*min + i),(*min + i + 1));
      		for (int j=0;j<comm_sz;j++){
      			if (a[i + 10*j] == 1)
      				printf("X");
      		}
      		printf("\n");
      	}
      	free(a);
    } 
    else {
    	MPI_Gather(local_total,bin_count,MPI_INT,a,bin_count,MPI_INT,0,MPI_COMM_WORLD);
   	}
}
