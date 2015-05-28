
#include <stdio.h>
#include <string.h>

#include <mpi.h>


const int N = 4;
const int NUMBER_OF_NODES = N;


int MyMPIInit(int argc, char **argv);
void multiplicationSerial(const int matrix[N][N], const int inputVector[N]);
void multiplicationParallel(int currentProcessRank, const int matrixRow[N], const int inputVector[N]);
int calculateOutputVectorElement(const int matrixRow[N], const int inputVector[N]);
void printVector(const int vector[N], const char* title);
//void cleanVector(int vector[]);


int main(int argc, char **argv)
{

	int currentProcessRank = MyMPIInit(argc, argv);
	printf("myrank = %d\n\n", currentProcessRank);


	const int matrix[N][N] =
	{
		{ 3, 4, 1, 2 },
		{ 0, 1, 3, 4 },
		{ 2, 1, 3, 1 },
		{ 3, 0, 1, 2 }
	};

	const int inputVector[N] = { 4, 1, 3, 2 };
	
	/*if (currentProcessRank == 0)
	{
		multiplicationSerial(matrix, inputVector);
	}*/
	
	int matrixRow[N];
	memcpy(matrixRow, matrix[currentProcessRank], N * sizeof(int));

	multiplicationParallel(currentProcessRank, matrixRow, inputVector);

	printf("----------------------------------------------------------------------------\n");

	MPI_Finalize();

	return 0;
}


int MyMPIInit(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int currentProcessRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &currentProcessRank);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	return currentProcessRank;
}


void multiplicationSerial(const int matrix[N][N], const int inputVector[N])
{
	int outputVector[N];

	for (int i = 0; i < N; i++)
	{
		outputVector[i] = 0;

		for (int j = 0; j < N; j++)
		{
			outputVector[i] += matrix[i][j] * inputVector[j];
		}
	}

	printVector(outputVector, "Output vector - serial algorithm");
}


void multiplicationParallel(int currentProcessRank, const int matrixRow[N], const int inputVector[N])
{
	
	//printVector(matrixRow, "Matrix - Row");


	//int inputVectorForNode[N];

	////this is the element of the input vector that is initially deployed at the current node
	//inputVectorForNode[currentProcessRank] = inputVector[currentProcessRank];

	////all-to-all broadcast of the initial input vector element at each node
	//for (int i = 0; i < N; i++)
	//{
	//	MPI_Bcast(&inputVectorForNode[i], 1, MPI_INT, i, MPI_COMM_WORLD);
	//}

	//printVector(inputVectorForNode, "Input vector - from broadcast");


	int outputVectorElement = calculateOutputVectorElement(matrixRow, inputVector);
	printf("output[%d] : %d\n", currentProcessRank, outputVectorElement);
}


int calculateOutputVectorElement(const int matrixRow[N], const int inputVector[N])
{
	int outputVectorElement = 0;

	for (int i = 0; i < N; i++)
	{
		outputVectorElement += matrixRow[i] * inputVector[i];
	}

	return outputVectorElement;
}


void printVector(const int vector[N], const char* title)
{
	printf(title);
	printf("\n");

	for (int i = 0; i < N; i++)
	{
		printf("%d\n", vector[i]);
	}

	printf("\n");
}


//void cleanVector(int vector[])
//{
//	;
//	for (int i = 0; i < N; i++)
//	{
//		vector[i] = -1;
//	}
//}

//const int matrix[N][N] =
//{
//	{ 3, 1, 2 },
//	{ 4, 0, 1 },
//	{ 2, 1, 3 }
//};
//
//const int inputVector[N] = { 3, 1, 2 };