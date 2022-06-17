#include "RCPSP.h"


RCPSP::RCPSP(int jobs, int horizon, int numrenewable, std::vector<activity> activities, std::vector<int> resourceavailability)
{
	this->jobs = jobs;
	this->horizon = horizon;
	this->numrenewable = numrenewable;
	this->activities = activities;
	this->resourceavailability = resourceavailability;
}

