
#include <stdio.h>
#include <string.h>

#include <mpi.h>

void processFirst(int current_process_rank, int count);
void processSecond(int current_process_rank, int count);


int main(int argc, char **argv)
{
	//printf("Simple transfer - addition");


	MPI_Init(&argc, &argv);

	int current_process_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	printf("myrank = %d\n", current_process_rank);


	int count = 1;

	if (current_process_rank == 0)
	{
		processFirst(current_process_rank, count);
	}
	else if (current_process_rank == 1)
	{
		processSecond(current_process_rank, count);
	}

	MPI_Finalize();

	return 0;
}


void processFirst(int current_process_rank, int count)
{
	int number_to_send = 8;
	int destination_process_rank = 1;

	MPI_Send(&number_to_send, count, MPI_INT, destination_process_rank, 0, MPI_COMM_WORLD);

	printf("Process %d sent number %d to process %d\n", current_process_rank, number_to_send, destination_process_rank);


	int number_received;
	int source_process_rank = 1;

	MPI_Recv(&number_received, count, MPI_INT, source_process_rank, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	printf("Process %d received number %d from process %d\n", current_process_rank, number_received, source_process_rank);
}

void processSecond(int current_process_rank, int count)
{
	int source_process_rank = 0;
	int number_received;

	MPI_Recv(&number_received, count, MPI_INT, source_process_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	printf("Process %d received number %d from process %d\n", current_process_rank, number_received, source_process_rank);


	int number_to_add = 5;
	int number_to_send = number_received + number_to_add;
	int destination_process_rank = source_process_rank;

	printf("Process %d added %d to the received number (%d) and will send the sum (%d) back to process %d\n",
		current_process_rank, number_to_add, number_received, number_to_send, destination_process_rank);

	MPI_Send(&number_to_send, count, MPI_INT, destination_process_rank, 1, MPI_COMM_WORLD);

}





