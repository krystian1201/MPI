
#include <stdio.h>
#include <string.h>

#include <mpi.h>


int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	//printf("start\n");


	int current_process_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	printf("myrank = %d\n", current_process_rank);

	int number;
	if (current_process_rank == 0)
	{
		number = 10;
		MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
	}
	else if (current_process_rank == 1)
	{
		MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
			MPI_STATUS_IGNORE);

		printf("Process 1 received number %d from process 0\n", number);
	}

	MPI_Finalize();

	return 0;
}
