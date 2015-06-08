
#include "MPIInfo.h"


MPIInfo::MPIInfo(int currentProcessRank, int worldSize)
{
	this->currentProcessRank = currentProcessRank;
	this->worldSize = worldSize;
}

int MPIInfo::getCurrentProcessRank()
{
	return currentProcessRank;
}

int MPIInfo::getWorldSize()
{
	return worldSize;
}


MPIInfo::~MPIInfo()
{

}
