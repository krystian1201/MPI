

#include <stdio.h>
#include <string.h>

#include <mpi.h>

#include "FinalMessage.h"


const int NUMBER_OF_NODES = 4;

int MyMPIInit(int argc, char **argv);
void startAllToAllCommunication(int currentProcessRank, bool isDebugMode);
void populateInitialArrayToSend(int sendArray[], int currentProcessRank);
void sendMessage(int currentProcessRank, int sendArray[], int step, bool isDebugMode);
void receiveMessage(int currentProcessRank, int receiveArray[], int step, FinalMessage messagesAtDestination[NUMBER_OF_NODES - 1], bool isDebugMode);
void takeMessageForTheNodeFromReceivedArray(int receiveArray[], int step, int currentProcessRank, 
	FinalMessage messagesAtDestination[NUMBER_OF_NODES - 1], bool isDebugMode);
int calculateIndexOfMessageForCurrentNode(int currentProcessRank, int step);
int calculateDestinationProcessRank(int currentProcessRank);
int calculateSourceProcessRank(int currentProcessRank);
int calculateOriginalSourceProcessRank(int currentProcessRank, int step);
void printArray(int array[], int count, const char* title);
void printMessagesAtDestinations(FinalMessage messagesAtDestination[NUMBER_OF_NODES - 1]);
int mod(int a, int b);




int main(int argc, char **argv)
{

	int currentProcessRank = MyMPIInit(argc, argv);
	printf("myrank = %d\n\n", currentProcessRank);


	startAllToAllCommunication(currentProcessRank, false);

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



void startAllToAllCommunication(int currentProcessRank, bool isDebugMode)
{

	int sendArray[NUMBER_OF_NODES - 1];
	populateInitialArrayToSend(sendArray, currentProcessRank);


	int receiveArray[NUMBER_OF_NODES - 1];
	//int messagesAtDestination[NUMBER_OF_NODES - 1];
	FinalMessage messagesAtDestination[NUMBER_OF_NODES - 1];

	for (int step = 0; step < NUMBER_OF_NODES - 1; step++)
	{
		if (isDebugMode)
		{
			printf("Step %d\n\n", step);
		}

		//number of messages to send decreases with each step
		const int msgsCount = NUMBER_OF_NODES - 1 - step;


		sendMessage(currentProcessRank, sendArray, step, false);
		 
		receiveMessage(currentProcessRank, receiveArray, step, messagesAtDestination, false);

		//node sends in the next step the messages it received in the current step
		memcpy(sendArray, receiveArray, msgsCount * sizeof(int));

		printf("*********************************************************\n");
	}

	//printArray(messagesAtDestination, NUMBER_OF_NODES - 1, "Messages meant for the current node");
	printMessagesAtDestinations(messagesAtDestination);
}


void populateInitialArrayToSend(int sendArray[NUMBER_OF_NODES - 1], int currentProcessRank)
{
	int array_index = 0;

	for (int dest_node = 0; dest_node < NUMBER_OF_NODES; dest_node++)
	{
		//node doesn't send message to itself
		if (dest_node != currentProcessRank)
		{
			sendArray[array_index] = currentProcessRank * 1000 + dest_node;

			array_index++;
		}
	}


	/*switch (currentProcessRank)
	{
		case 0:
			sendArray[0] = 7;
			sendArray[1] = 4;
			sendArray[2] = 5;

			break;

		case 1:
			sendArray[0] = 6;
			sendArray[1] = 8;
			sendArray[2] = 9;

			break;

		case 2:
			sendArray[0] = 9;
			sendArray[1] = 7;
			sendArray[2] = 6;

			break;

		case 3:
			sendArray[0] = 5;
			sendArray[1] = 7;
			sendArray[2] = 9;

			break;
	}*/
}


void sendMessage(int currentProcessRank, int sendArray[], int step, bool isDebugMode)
{
	int destinationProcessRank = calculateDestinationProcessRank(currentProcessRank);
	int msgsCount = NUMBER_OF_NODES - 1 - step;

	//tag - indicates step (probably it's not necessary
	MPI_Send(sendArray, msgsCount, MPI_INT, destinationProcessRank, step, MPI_COMM_WORLD);

	if (isDebugMode)
	{
		printf("Process_%d sent array to process_%d\n", currentProcessRank, destinationProcessRank);

		printArray(sendArray, msgsCount, "Send array");
		printf("\n");
	}
}


void receiveMessage(int currentProcessRank, int receiveArray[], int step, FinalMessage messagesAtDestination[NUMBER_OF_NODES - 1], bool isDebugMode)
{
	int sourceProcessRank = calculateSourceProcessRank(currentProcessRank);
	int msgsCount = NUMBER_OF_NODES - 1 - step;

	//step number is used as a tag
	MPI_Recv(receiveArray, msgsCount, MPI_INT, sourceProcessRank, step, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	if (isDebugMode)
	{
		printf("Process_%d received array from process_%d\n", currentProcessRank, sourceProcessRank);
		printArray(receiveArray, msgsCount, "Receive array - just as received");
		//printf("\n");
	}

	//each node takes from the array it received the message that was meant for this node
	takeMessageForTheNodeFromReceivedArray(receiveArray, step,
		currentProcessRank, messagesAtDestination, false);
}


void takeMessageForTheNodeFromReceivedArray(int receiveArray[], int step,
	int currentProcessRank, FinalMessage messagesAtDestination[NUMBER_OF_NODES - 1], bool isDebugMode)
{

	int indexOfMessageForCurrentNode = calculateIndexOfMessageForCurrentNode(currentProcessRank, step);
	int msgsCount = NUMBER_OF_NODES - 1 - step;


	int originalSourceProcess = calculateOriginalSourceProcessRank(currentProcessRank, step);

	int array_index = 0;


	for (int i = 0; i < msgsCount; i++)
	{

		//node will not send further message which was meant for it
		//it will keep it
		if (i != indexOfMessageForCurrentNode)
		{
			receiveArray[array_index] = receiveArray[i];
			array_index++;
		}
		else
		{
			//messagesAtDestination[currentProcessRank][step] = receiveArray[i];
			//messagesAtDestination[step] = receiveArray[i];
			messagesAtDestination[step] = FinalMessage(originalSourceProcess, receiveArray[i]);
		}
	}

	if (isDebugMode)
	{
		printArray(receiveArray, msgsCount - 1, "Receive array - after message for the node was taken");
	}
}


int calculateIndexOfMessageForCurrentNode(int currentProcessRank, int step)
{
	int index = -1;

	if (currentProcessRank > 1 + step)
	{
		index = currentProcessRank - 1 - step;
	}
	else
	{
		index = 0;
	}

	return index;

}


int calculateDestinationProcessRank(int currentProcessRank)
{
	return mod((currentProcessRank + 1), NUMBER_OF_NODES);
	
}


int calculateSourceProcessRank(int currentProcessRank)
{
	return mod((currentProcessRank - 1), NUMBER_OF_NODES);
}


int calculateOriginalSourceProcessRank(int currentProcessRank, int step)
{
	return mod(currentProcessRank - 1 - step, NUMBER_OF_NODES);
}


void printArray(int array[], int count, const char* title)
{
	printf("\n");
	printf(title);
	printf("\n");

	for (int i = 0; i < count; i++)
	{
		printf("%d %d\n", i, array[i]);
	}

	printf("\n");
}


void printMessagesAtDestinations(FinalMessage messagesAtDestination[NUMBER_OF_NODES - 1])
{
	for (int i = 0; i < NUMBER_OF_NODES - 1; i++)
	{
		messagesAtDestination[i].print();
	}
}


int mod(int a, int b)
{
	int r = a % b;
	return r < 0 ? r + b : r;
}


//void printMessagesAtDestinations(int array[NUMBER_OF_NODES][NUMBER_OF_NODES - 1])
//{
//	printf("\n");
//	printf("Messages at destinations");
//	printf("\n");
//
//	for (int i = 0; i < NUMBER_OF_NODES; i++)
//	{
//		for (int j = 0; i < NUMBER_OF_NODES - 1; i++)
//		{
//			printf("%d\t", array[i][j]);
//		}
//
//		printf("\n");
//	}
//
//	printf("\n");
//}

//void cleanArray(int array[])
//{
//	int i;
//	for (i = 0; i < NUMBER_OF_NODES - 1; i++)
//	{
//		array[i] = -1;
//	}
//}

//void stepZeroReceive(int currentProcessRank, int receiveArray[], int messagesAtDestination[NUMBER_OF_NODES][NUMBER_OF_NODES - 1], bool isDebugMode)
//{
//	int sourceProcessRank = calculateSourceProcessRank(currentProcessRank);
//
//	MPI_Recv(receiveArray, NUMBER_OF_NODES - 1, MPI_INT, sourceProcessRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//
//
//	if (isDebugMode)
//	{
//		printf("Process_%d received array from process_%d\n\n", currentProcessRank, sourceProcessRank);
//		printArray(receiveArray, NUMBER_OF_NODES - 1, "Receive array - just as received");
//		printf("\n");
//	}
//
//
//	int msgsCount = NUMBER_OF_NODES - 1;
//	int step = 0;
//
//	takeMessageForTheNodeFromReceivedArray(receiveArray, msgsCount, step,
//		currentProcessRank, messagesAtDestination, true);
//}


//void populateArrayToSendFromReceivedArray(int sendArray[], int receiveArray[], int step, int msgsCount,
//	int currentProcessRank)
//{
//	cleanArray(sendArray);
//
//
//	int indexOfMessageForCurrentNode = calculateIndexOfMessageForCurrentNode(currentProcessRank, step);
//
//	printf("indexOfMessageForCurrentNode: %d\n\n", indexOfMessageForCurrentNode);
//
//
//	int array_index = 0;
//
//	//msgsCount means the number of messages to be sent in the current step
//	//it is less by 1 than the number of messages received in the previous step
//	for (int i = 0; i < msgsCount + 1; i++)
//	{
//		
//		//node will not send further message which was meant for it
//		//it will keep it
//		if (i != indexOfMessageForCurrentNode)
//		{
//			sendArray[array_index] = receiveArray[array_index];
//			array_index++;
//		}	
//	}
//}

//void stepZeroSend(int currentProcessRank, bool isDebugMode)
//{
//	int destinationProcessRank = calculateDestinationProcessRank(currentProcessRank);
//
//	//initial messages to send
//	int sendArray[NUMBER_OF_NODES - 1];
//	populateInitialArrayToSend(sendArray, currentProcessRank);
//
//
//	//tag - indicates step?
//	MPI_Send(sendArray, NUMBER_OF_NODES - 1, MPI_INT, destinationProcessRank, 0, MPI_COMM_WORLD);
//
//
//	if (isDebugMode)
//	{
//		printf("Process_%d sent array to process_%d\n", currentProcessRank, destinationProcessRank);
//		printArray(sendArray, NUMBER_OF_NODES - 1, "Step 0 - Send array");
//		printf("\n");
//	}
//
//}