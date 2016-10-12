/*
   Sum of Prefix
   Author: Carlos Galdamez
   Description: Serial program that finds prefix sums of a list of numbers. For example:

   If we have the list : 1 2 3 4 , then the prefix sums list would be 1, 1+2, 1+2+3, 1+2+3+4.
*/

#include <stdlib.h>
#include <stdio.h>

void getN (int* n);
void getElements (int* elements, int n);
void printPrefixSum (int* prefixSums, int num );

int main (void){
	int numberOfElements;
	int* elements;
	int* prefixSums;
	
	// Ask user for number of elements that will be entered
	getN(&numberOfElements);

	// Allocate memory to hold list values and prefix sums
	elements = malloc(numberOfElements * sizeof(int));
	prefixSums = malloc(numberOfElements * sizeof(int));

	// Ask use for elements of the list
	getElements(elements,numberOfElements);

	// The following creates the list of sum prefixes
	int sum = 0;
	for (int i = 0; i < numberOfElements; i++){
		sum += elements[i];
		prefixSums[i] = sum;
	}

	// Display results
	printPrefixSum(prefixSums,numberOfElements);

	free(prefixSums);
	free(elements);
	return 0;
}

void getN (int* n){
	printf("How many element do you want to get the prefix sums for?\n");
	scanf("%d",n);
}

void getElements (int* elements, int n){

	printf("Enter %d integers:\n", n);
	for (int i = 0; i < n; i++)
		scanf("%d", &elements[i]);

}

void printPrefixSum (int* prefixSums, int num ){

	printf("Prefix Sums: ");
	for (int i = 0; i < num; i++)
		printf("%d ",prefixSums[i]);
	printf("\n");

}