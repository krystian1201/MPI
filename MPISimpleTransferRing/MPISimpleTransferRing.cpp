

#include <stdio.h>
#include <string.h>

#include <mpi.h>

void processZero();
void processOneAndGreater(int current_process_rank);

const int NUMBER_OF_NODES = 4;
const int COUNT = 1;


int main(int argc, char **argv)
{

	MPI_Init(&argc, &argv);

	int current_process_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	printf("myrank = %d\n", current_process_rank);

	if (current_process_rank == 0)
	{
		processZero();
	}
	else if (current_process_rank > 0 && current_process_rank < NUMBER_OF_NODES)
	{
		processOneAndGreater(current_process_rank);
	}


	MPI_Finalize();

	return 0;
}


void processZero()
{
	int number_to_send = 7;
	int destination_process_rank = 1;

	MPI_Send(&number_to_send, COUNT, MPI_INT, destination_process_rank, 0, MPI_COMM_WORLD);

	printf("Process 0 sent number %d to process %d\n", number_to_send, destination_process_rank);


	int number_received;
	int source_process_rank = NUMBER_OF_NODES - 1;

	MPI_Recv(&number_received, COUNT, MPI_INT, source_process_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	printf("Process 0 received number %d from process %d\n", number_received, source_process_rank);
}

void processOneAndGreater(int current_process_rank)
{
	int number_received;
	int source_process_rank = (current_process_rank - 1) % NUMBER_OF_NODES;

	MPI_Recv(&number_received, COUNT, MPI_INT, source_process_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	printf("Process %d received number %d from process %d\n", current_process_rank, number_received, source_process_rank);

	int number_to_send = number_received;
	int destination_process_rank = (current_process_rank + 1) % NUMBER_OF_NODES;

	MPI_Send(&number_to_send, COUNT, MPI_INT, destination_process_rank, 0, MPI_COMM_WORLD);

	printf("Process %d sent number %d to process %d\n", current_process_rank, number_to_send, destination_process_rank);
}



