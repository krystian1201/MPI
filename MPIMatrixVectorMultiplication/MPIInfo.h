#pragma once

class MPIInfo
{
	private:
		int currentProcessRank;
		int worldSize;

	public:
		MPIInfo(int currentProcessRank, int world_size);
		~MPIInfo();
		int getCurrentProcessRank();
		int getWorldSize();
};

