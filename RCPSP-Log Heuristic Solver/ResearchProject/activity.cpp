#include "activity.h"

activity::activity(int number, int duration, std::vector<int> resourceneeds, std::vector<int> successors, int type)
{
	this->number = number;
	this->duration = duration;
	this->resourceneeds = resourceneeds;
	this->successors = successors;
	this->type = type;
	this->heur = -1;
}

bool activity::equals(activity &other)
{
	return this->number == other.number;
}

void activity::addPredecessor(int predecessor)
{
	this->predecessors.push_back(predecessor);
}
