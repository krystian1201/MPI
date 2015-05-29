
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <ctime> 

#include <mpi.h>
#include "MPIInfo.h"

const int MAT_ARR_SIZE = 10000;


MPIInfo MyMPIInit(int argc, char **argv);
void MultiplicationSerial(const int matrix[MAT_ARR_SIZE][MAT_ARR_SIZE], const int inputVector[MAT_ARR_SIZE]);
void MultiplicationParallel(int currentProcessRank, const int matrix[MAT_ARR_SIZE][MAT_ARR_SIZE],
	const int inputVector[MAT_ARR_SIZE], int numberOfNodes);
int CalculateOutputVectorElement(const int matrixRow[MAT_ARR_SIZE], const int inputVector[MAT_ARR_SIZE]);
void PopulateTestInputVector(int inputVector[MAT_ARR_SIZE]);
void PopulateTestMatrix(int matrix[MAT_ARR_SIZE][MAT_ARR_SIZE]);
void GenerateRandomArray(int array[MAT_ARR_SIZE], int minElement, int range);
void GenerateRandomMatrix(int matrix[MAT_ARR_SIZE][MAT_ARR_SIZE], int minElement, int range);
void PrintVector(const int vector[MAT_ARR_SIZE], const char* title);
void PrintMatrix(const int matrix[MAT_ARR_SIZE][MAT_ARR_SIZE], const char* title);


int matrix[MAT_ARR_SIZE][MAT_ARR_SIZE];
int inputVector[MAT_ARR_SIZE];


int main(int argc, char **argv)
{

	MPIInfo mpiInfo = MyMPIInit(argc, argv);
	int currentProcessRank = mpiInfo.getCurrentProcessRank();
	int numberOfNodes = mpiInfo.getWorldSize();

	printf("myrank = %d\n\n", currentProcessRank);


	/*PopulateTestMatrix(matrix);
	PopulateTestInputVector(inputVector);*/
	
	
	const int minRandomNumber = 0;
	const int range = 100;
	GenerateRandomArray(inputVector, minRandomNumber, range);
	GenerateRandomMatrix(matrix, minRandomNumber, range);
	
	//MPI_Barrier(MPI_COMM_WORLD);

	//PrintVector(inputVector, "Random input vector:");
	//PrintMatrix(matrix, "Random matrix:");


	clock_t clockBegin = clock();

		/*if (currentProcessRank == 0)
		{
			MultiplicationSerial(matrix, inputVector);
		}*/

		MultiplicationParallel(currentProcessRank, matrix, inputVector, numberOfNodes);

	clock_t clockEnd = clock();
	double elapsedTime = double(clockEnd - clockBegin) / CLOCKS_PER_SEC;

	printf("time: %f\n", elapsedTime);

	printf("----------------------------------------------------------------------------\n");

	MPI_Finalize();

	return 0;
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


void MultiplicationSerial(const int matrix[MAT_ARR_SIZE][MAT_ARR_SIZE], const int inputVector[MAT_ARR_SIZE])
{
	int outputVector[MAT_ARR_SIZE];

	for (int row = 0; row < MAT_ARR_SIZE; row++)
	{
		outputVector[row] = 0;

		for (int col = 0; col < MAT_ARR_SIZE; col++)
		{
			outputVector[row] += matrix[row][col] * inputVector[col];
		}
	}

	//PrintVector(outputVector, "Output vector - serial algorithm");
}


void MultiplicationParallel(int currentProcessRank, const int matrix[MAT_ARR_SIZE][MAT_ARR_SIZE], 
	const int inputVector[MAT_ARR_SIZE], int numberOfNodes)
{
	int remainder = MAT_ARR_SIZE % numberOfNodes;

	if (remainder != 0)
	{
		printf("Matrix - Array size: %d is not divisible by the number of nodes: %d\n", MAT_ARR_SIZE, numberOfNodes);
		return;
	}
	
	//PrintVector(matrixRow, "Matrix - Row");

	int elementsPerNode = (int)(MAT_ARR_SIZE / numberOfNodes);

	int startNode = currentProcessRank * elementsPerNode;

	//printf("elems per node: %d\n", elementsPerNode);
	//printf("start node: %d\n", startNode);


	int inputVectorForNode[MAT_ARR_SIZE];

	for (int node = startNode; node < startNode + elementsPerNode; node++)
	{
		//printf("node: %d\n", node);

		//this is the element of the input vector that is initially deployed at the current node
		inputVectorForNode[node] = inputVector[node];
	}

	//all-to-all broadcast of the initial input vector
	//number of elements each node sends depends on the number of nodes
	//for example for matrix size 8 and 4 nodes each node sends 2 elements
	//node receives input vector elements from all other nodes
	//and sends its input vector element to all other nodes
	for (int i = 0; i < MAT_ARR_SIZE; i++)
	{
		int broadcastRoot = int(i / elementsPerNode);

		//printf("input elem: %d broadcastRoot: %d\n", i, broadcastRoot);

		MPI_Bcast(&inputVectorForNode[i], 1, MPI_INT, broadcastRoot, MPI_COMM_WORLD);
	}


	//PrintVector(inputVectorForNode, "Input vector - from broadcast");


	for (int node = startNode; node < startNode + elementsPerNode; node++)
	{
		int outputVectorElement = CalculateOutputVectorElement(matrix[node], inputVectorForNode);

		//printf("output[%d] : %d\n", node, outputVectorElement);
	}

}


int CalculateOutputVectorElement(const int matrixRow[MAT_ARR_SIZE], const int inputVector[MAT_ARR_SIZE])
{
	int outputVectorElement = 0;

	for (int i = 0; i < MAT_ARR_SIZE; i++)
	{
		outputVectorElement += matrixRow[i] * inputVector[i];
	}

	return outputVectorElement;
}

void PopulateTestInputVector(int inputVector[MAT_ARR_SIZE])
{
	//const int inputVector[MAT_ARR_SIZE] = { 4, 1, 3, 2 };
	int inputVectorIntialized[MAT_ARR_SIZE] = { 8, 7, 7, 2, 6, 8, 5, 4 };

	for (int i = 0; i < MAT_ARR_SIZE; i++)
	{
		inputVector[i] = inputVectorIntialized[i];
	}
}


void PopulateTestMatrix(int matrix[MAT_ARR_SIZE][MAT_ARR_SIZE])
{
	/*const int matrix[MAT_ARR_SIZE][MAT_ARR_SIZE] =
	{
	{ 3, 4, 1, 2 },
	{ 0, 1, 3, 4 },
	{ 2, 1, 3, 1 },
	{ 3, 0, 1, 2 }
	};*/

	const int matrixInitialized[MAT_ARR_SIZE][MAT_ARR_SIZE] =
	{
		{ 0, 9, 2, 5, 6, 2, 5, 2 },
		{ 0, 2, 3, 1, 9, 4, 7, 7 },
		{ 9, 0, 4, 2, 7, 8, 0, 4 },
		{ 3, 7, 2, 1, 1, 7, 8, 4 },
		{ 5, 2, 2, 3, 0, 3, 0, 0 },
		{ 4, 3, 7, 0, 0, 6, 8, 8 },
		{ 3, 9, 2, 6, 7, 9, 6, 7 },
		{ 3, 3, 7, 9, 6, 8, 6, 6 }
	};

	for (int i = 0; i < MAT_ARR_SIZE; i++)
	{
		for (int j = 0; j < MAT_ARR_SIZE; j++)
		{
			matrix[i][j] = matrixInitialized[i][j];
		}
	}
}

void GenerateRandomArray(int array[MAT_ARR_SIZE], int minElement, int range)
{
	srand((unsigned)time(0));

	//printf("rand max: %d\n", RAND_MAX);

	for (int i = 0; i < MAT_ARR_SIZE; i++)
	{
		array[i] = rand() % range + minElement;
	}
}


void GenerateRandomMatrix(int matrix[MAT_ARR_SIZE][MAT_ARR_SIZE], int minElement, int range)
{
	srand((unsigned)time(0));

	//printf("rand max: %d\n", RAND_MAX);

	for (int i = 0; i < MAT_ARR_SIZE; i++)
	{
		for (int j = 0; j < MAT_ARR_SIZE; j++)
		{
			matrix[i][j] = rand() % range + minElement;
		}

		//GenerateRandomArray(matrix[i], minElement, range);
	}
}


void PrintVector(const int vector[MAT_ARR_SIZE], const char* title)
{
	printf(title);
	printf("\n");

	for (int i = 0; i < MAT_ARR_SIZE; i++)
	{
		printf("%d\t", vector[i]);
	}

	printf("\n\n");
}


void PrintMatrix(const int matrix[MAT_ARR_SIZE][MAT_ARR_SIZE], const char* title)
{
	printf(title);
	printf("\n");

	for (int i = 0; i < MAT_ARR_SIZE; i++)
	{
		PrintVector(matrix[i], "");
		printf("\n");
	}

	printf("\n\n");
}
