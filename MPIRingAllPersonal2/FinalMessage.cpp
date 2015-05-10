
#include <stdio.h>

#include "FinalMessage.h"


FinalMessage::FinalMessage()
{
	sourceProcessRank = -1;
	value = -1;
}

FinalMessage::FinalMessage(int sourceNode, int val)
{
	sourceProcessRank = sourceNode;
	value = val;
}

void FinalMessage::print()
{
	printf("source process: %d value: %d\n", sourceProcessRank, value);
}