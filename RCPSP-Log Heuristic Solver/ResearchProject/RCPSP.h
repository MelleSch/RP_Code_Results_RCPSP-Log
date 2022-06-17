#ifndef RCPSP_h
#define RCPSP_h
#include <vector>
#include "activity.h"

class RCPSP {
public:
	int jobs;
	int horizon;
	int numrenewable;
	std::vector<activity> activities;
	std::vector<int> resourceavailability;
	RCPSP(int jobs, int horizon, int numrenewable, std::vector<activity> activities, std::vector<int> resourceavailability);
};

#endif