#include "Record.h"

const auto ScoreFilePath = L"files/score.dat";

void Record::init()
{
	if (FileSystem::Exists(ScoreFilePath))
	{
		BinaryReader reader(ScoreFilePath);

		reader.read(&this->recorded, sizeof(int));
	}
	else this->recorded = 0;
	this->is_updated = false;
}
int Record::getCurrentMaxScore()
{
	return this->recorded;
}
void Record::recordScore(int s)
{
	this->is_updated = this->recorded < s;
	
	if (this->is_updated)
	{
		this->recorded = s;

		// Reflect
		BinaryWriter writer(ScoreFilePath);
		writer.write(this->recorded);
	}
}