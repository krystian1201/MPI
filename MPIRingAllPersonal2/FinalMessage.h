

class FinalMessage
{
private:

	int sourceProcessRank;
	int value;

public:

	FinalMessage();
	FinalMessage(int sourceProcessRank, int val);
	void print();

};