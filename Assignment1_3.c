#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>


void readN(int* n, int* local_n, int my_rank, int comm_sz);
void readList(double* list, double* local_list, int* min, int* max,int* bin_count, int n, int local_n, int my_rank, int comm_sz);
void printLocalList(double* local_list, int local_n,int my_rank);
void count(int* local_total,double* local_list, int* min, int local_n, int my_rank, int comm_sz);
void initialize_count(int* local_total, int local_n);
void printHistogram(int* local_total, int local_n, int bin_count, int* min, int my_rank, int comm_sz);

int main(void){
	
	int my_rank,comm_sz;
	int n,local_n;
	int min,max;
	double *list, *local_list;
	int bin_count;

	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	readN(&n,&local_n,my_rank,comm_sz);

	//printf("rank %d, n_value: %d\n",my_rank,local_n);

	list = malloc(n * sizeof(double));
	local_list = malloc(local_n * sizeof(double));

	readList(list,local_list,&min,&max, &bin_count, n,local_n,my_rank,comm_sz);


	//if(my_rank == 0) printLocalList(local_list,local_n,my_rank);

	//if(my_rank == 0){
	//	printf("max = %d\n",max);
	//	printf("min = %d\n",min);
	//	printf("bins = %d\n",bin_count);
	//}

	int total[bin_count], local_total[bin_count];

	initialize_count(local_total,bin_count);
	count(local_total,local_list,&min,local_n,my_rank,comm_sz);

	//if(my_rank == 0){
	//	for (int i = 0; i < bin_count; i++){
	//		printf("%d\t",local_total[i]);
	//	}
	//
	//	printf("\n");
	//}
	printHistogram(local_total,local_n,bin_count,&min,my_rank,comm_sz);

	free(list);
	free(local_list);
	MPI_Finalize();
	return 0;
}

void initialize_count(int* local_total, int bin_count){
	for(int i = 0; i < bin_count;i++){
		local_total[i] = 0;
	}
}

void readN(int* n, int* local_n, int my_rank, int comm_sz){

	if(my_rank == 0){
		printf("How many elements are being represented by the histogram?\n");
		scanf("%d", n);
	}

	MPI_Bcast(n,1,MPI_INT,0,MPI_COMM_WORLD);

	*local_n = *n/comm_sz;

}

void readList(double* list, double* local_list, int* min, int* max, int* bin_count, int n, int local_n, int my_rank, int comm_sz){

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
		MPI_Scatter(list,local_n,MPI_DOUBLE,local_list,local_n,MPI_DOUBLE,0,MPI_COMM_WORLD);
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


void printLocalList(double* local_list, int local_n,int my_rank){
	for (int i = 0; i < local_n; i++){
		printf("my rank: %d, and my list: %lf ",my_rank,local_list[i]);
	}

	printf("\n");
}

void count(int* local_total,double* local_list, int* min, int local_n, int my_rank, int comm_sz){
	for(int i = 0; i < local_n;i++){
		int index = floor(local_list[i] - *min);
		local_total[index] += 1;
		if(my_rank == 2){
			printf("%d\n",index );
		}
	}
}

void printHistogram(int* local_total, int local_n, int bin_count, int* min, int my_rank, int comm_sz){
	int* a = NULL;
   	int i;
   
   	if (my_rank == 0) {
      	a = malloc(bin_count * comm_sz * sizeof(int));
    	MPI_Gather(local_total,bin_count,MPI_INT,a,bin_count,MPI_INT,0,MPI_COMM_WORLD);
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
