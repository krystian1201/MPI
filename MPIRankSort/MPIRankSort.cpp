
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <ctime> 

#include <mpi.h>
#include "MPIInfo.h"

//const int ARRAY_SIZE = 16777216;
const int ARRAY_SIZE = 40000;


MPIInfo MyMPIInit(int argc, char **argv);
void RankSortSequential(const int unsortedArray[ARRAY_SIZE], int sortedArray[ARRAY_SIZE]);
void RankSortParallel(const int unsortedArray[ARRAY_SIZE], int sortedArray[ARRAY_SIZE], 
	int currentProcessRank, int numberOfNodes);
int FindRank(const int array[ARRAY_SIZE], int index);
void GenerateRandomArray(int array[ARRAY_SIZE], int minElement, int range);
void PrintArray(const int array[ARRAY_SIZE], const char* title);


int main(int argc, char **argv)
{
	MPIInfo mpiInfo = MyMPIInit(argc, argv);

	int currentProcessRank = mpiInfo.getCurrentProcessRank();
	printf("myrank = %d\n\n", currentProcessRank);

	int numberOfNodes = mpiInfo.getWorldSize();
	//printf("number of processors: %d\n", numberOfNodes);


	//const int unsortedArray[ARRAY_SIZE] = { 22, 99, 77, 33, 55, 11, 44, 88 };
	int unsortedArray[ARRAY_SIZE];
	int sortedArray[ARRAY_SIZE];

	const int minRandomNumber = 0;
	const int range = 10000;
	GenerateRandomArray(unsortedArray, minRandomNumber, range);

	//PrintArray(unsortedArray, "Unsorted array:");

	//RankSortSequential(unsortedArray, sortedArray);
	//PrintArray(sortedArray, "Sorted array:");

	clock_t clockBegin = clock();

		RankSortParallel(unsortedArray, sortedArray, currentProcessRank, numberOfNodes);

	clock_t clockEnd = clock();
	double elapsedTime = double(clockEnd - clockBegin) / CLOCKS_PER_SEC;

	printf("time: %f\n", elapsedTime);

	printf("----------------------------------------------------------------------------\n");

	MPI_Finalize();

	return 0;
}


void GenerateRandomArray(int array[ARRAY_SIZE], int minElement, int range)
{
	srand((unsigned)time(0));

	//printf("rand max: %d\n", RAND_MAX);

	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		array[i] = rand() % range + minElement;
	}
}

MPIInfo MyMPIInit(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int currentProcessRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &currentProcessRank);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	MPIInfo mpiInfo = MPIInfo(currentProcessRank, world_size);

	return mpiInfo;
}


void RankSortSequential(const int unsortedArray[ARRAY_SIZE], int sortedArray[ARRAY_SIZE])
{
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		int rank = FindRank(unsortedArray, i);

		sortedArray[rank] = unsortedArray[i];
	}
}


void RankSortParallel(const int unsortedArray[ARRAY_SIZE], int sortedArray[ARRAY_SIZE], 
	int currentProcessRank, int numberOfNodes)
{
	int elementsPerNode = (int)(ARRAY_SIZE / numberOfNodes);

	//printf("elements per node: %d", elementsPerNode);


	int startIndex = currentProcessRank * elementsPerNode;

	for (int i = startIndex; i < startIndex + elementsPerNode; i++)
	{
		int rank = FindRank(unsortedArray, i);

		sortedArray[rank] = unsortedArray[i];

		//printf("%d: sortedArray[%d] = %d\n", i, rank, sortedArray[rank]);
	}

}


int FindRank(const int array[ARRAY_SIZE], int index)
{
	if (index >= ARRAY_SIZE)
	{
		return -1;
	}

	int rank = 0;

	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		if (array[i] < array[index])
		{
			rank++;
		}
	}

	return rank;

}

void PrintArray(const int array[ARRAY_SIZE], const char* title)
{
	printf(title);
	printf("\n");

	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		printf("%d: %d\n", i, array[i]);
	}

	printf("\n");
}