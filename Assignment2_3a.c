#include <stdlib.h>
#include <stdio.h>

void getN (int* n);
void getElements (int* elements, int n);
void printPrefixSum (int* prefixSums, int num );

int main (void){
	int numberOfElements;
	int* elements;
	int* prefixSums;
	
	getN(&numberOfElements);

	elements = malloc(numberOfElements * sizeof(int));
	prefixSums = malloc(numberOfElements * sizeof(int));

	getElements(elements,numberOfElements);

	int sum = 0;
	for (int i = 0; i < numberOfElements; i++){
		sum += elements[i];
		prefixSums[i] = sum;
	}

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