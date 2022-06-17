#ifndef solve_h
#define solve_h
#include <vector>
#include "RCPSP.h"
using namespace std;


int solveInstance(RCPSP& instance);
bool checkSchedulable(RCPSP& instance, activity& job, int time, vector<pair<int, activity>>& schedule);
int heur(RCPSP& instance, activity& job, vector<pair<int, activity>>& schedule);
int getNextTime(int time, vector<pair<int, activity>>& schedule);
bool validSchedule(RCPSP& instance, vector<pair<int, activity>>& schedule);
void printSchedule(vector<pair<int, activity>>& schedule);

class solve
{
	int minmakespan;
public:

};

#endif