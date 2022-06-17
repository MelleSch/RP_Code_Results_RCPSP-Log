#include "solve.h"
#include <iostream>
#include <string>

using namespace std;
int minmakespan = -1;
vector<activity> schedulable;
vector<int> resourceuse;
int changeovertime = 0;

int solveInstance(RCPSP &instance)
{
	// Initialise the global variables
	resourceuse = vector(instance.numrenewable, 0);
	int time = 0;
	minmakespan = 0;
	// Add the start activity to the schedule and add its successors to schedulable
	vector<pair<int, activity>> schedule;
	schedule.push_back(pair(time, instance.activities.front()));
	for (int i : instance.activities.front().successors) {
		schedulable.push_back(instance.activities[i-1]);
	}
	// Loop over the schedulable activities and determine which one has highest priority and can be scheduled
	while (!schedulable.empty()) {
		int index = -1;
		int maxheur = INT_MIN;
		for (int i = 0; i < schedulable.size(); i++) {
			if (schedulable[i].heur == -1)
			{
				schedulable[i].heur = heur(instance, schedulable[i], schedule);
			}
			if (schedulable[i].heur > maxheur) {
				if (!checkSchedulable(instance, schedulable[i], time, schedule)) continue;
				maxheur = schedulable[i].heur;
				index = i;
			}
		}
		// If no activity is schedulable, increment the time until the earliest finishing time
		if (index == -1) {
			time = getNextTime(time, schedule);
			continue;
		}
		// Otherwise add the activity to the schedule at the current time and add its successors to the schedulable vector
		activity nextjob = instance.activities[schedulable[index].number - 1];
		schedule.push_back(pair(time, nextjob));
		schedulable.erase(schedulable.begin() + index);
		for (int i = 0; i < resourceuse.size(); i++) {
			resourceuse[i] += nextjob.resourceneeds[i];
		}
		for (int successor : nextjob.successors) {
			bool addtoschedule = true;
			for (pair<int, activity> scheduledjob : schedule) {
				if (scheduledjob.second.number == successor) addtoschedule = false;
			}
			for (activity schedulablejob : schedulable) {
				if (schedulablejob.number == successor) addtoschedule = false;
			}
			if (addtoschedule)
			{
				schedulable.push_back(instance.activities[successor - 1]);
				for (int nextsuccessor : instance.activities[successor - 1].successors)
				{
					// If the successor of the scheduled job's successor has a BI relation it should also become schedulable.
					if (instance.activities[nextsuccessor - 1].type == 2)
					{
						bool nestaddtoschedule = true;
						for (pair<int, activity> nestscheduledjob : schedule)
						{
							if (nestscheduledjob.second.number == instance.activities[nextsuccessor - 1].successors[0]) nestaddtoschedule = false;
						}
						for (activity nestschedulablejob : schedulable)
						{
							if (nestschedulablejob.number == instance.activities[nextsuccessor - 1].successors[0]) nestaddtoschedule = false;
						}
						if (nestaddtoschedule) schedulable.push_back(instance.activities[nextsuccessor - 1]);
					}
				}
			}
		}
	}
	// Double check if the schedule is valid
	if(!validSchedule(instance, schedule)) std::cout << "ERROR" << std::endl;
	// Get the makespan of the schedule
	for (pair<int, activity> scheduledjob : schedule)
	{
		minmakespan = max(minmakespan, scheduledjob.first + scheduledjob.second.duration);
	}
	std::cout << to_string(minmakespan) << std::endl;
	
	//printSchedule(schedule);

	return minmakespan;
}

// Latest finish time using critical paths, this gives an optimistic makespan of the project and prioritizes jobs with successors that take long to finish.
int heur(RCPSP& instance, activity& job, vector<pair<int, activity>>& schedule)
{
	int criticalPath = 0;
	for (int i : job.successors) {
		activity successor = instance.activities[i - 1];
		criticalPath = max(criticalPath, heur(instance, successor, schedule));
	}
	return job.duration + criticalPath;
}

bool checkSchedulable(RCPSP& instance, activity& job, int time, vector<pair<int, activity>>& schedule)
{
	// Check if the necessary resources are available
	for (int i = 0; i < instance.numrenewable; i++) {
		resourceuse[i] += job.resourceneeds[i];
		if (resourceuse[i] > instance.resourceavailability[i]) {
			resourceuse[i] -= job.resourceneeds[i];
			return false;
		}
		resourceuse[i] -= job.resourceneeds[i];
	}

	// Check if the job has a BI relation with a successor that is currently active
	for (int successor : job.successors)
	{
		activity& succ = instance.activities[successor - 1];
		if (succ.type == 2) {
			if (succ.predecessors[0] == job.number) {
				for (pair<int, activity> scheduledjob : schedule)
				{
					if (successor == scheduledjob.second.number && scheduledjob.first + scheduledjob.second.duration + changeovertime > time) return false;
				}
			}
		}
	}

	if (job.type == 0) 
	{
		// Check if all predecessors have finished
		for (int predecessor : job.predecessors) 
		{
			bool contains = false;
			for (pair<int, activity> scheduledjob : schedule) 
			{
				if (scheduledjob.second.number != predecessor) continue;
				contains = true;
				if (scheduledjob.first + scheduledjob.second.duration > time) return false;
			}
			if (!contains) return false;
		}
		// If this is the end activity, check if all other activities have been scheduled.
		if (job.number == instance.jobs) {
			for (int i = 1; i < instance.jobs; i++)
			{
				bool contains = false;
				for (pair<int, activity> scheduledjob : schedule)
				{
					if (scheduledjob.second.number != i) continue;
					contains = true;
					if (scheduledjob.first + scheduledjob.second.duration > time) return false;
				}
				if (!contains) return false;
			}
		}
		return true;
	}

	if (job.type == 1) 
	{
		// Check if any predecessor has finished
		bool contains = false;
		for (int predecessor : job.predecessors) 
		{
			for (pair<int, activity> scheduledjob : schedule) 
			{
				if (scheduledjob.second.number != predecessor) continue;
				if (scheduledjob.first + scheduledjob.second.duration > time) continue;
				contains = true;
			}
		}
		if (!contains) return false;
		return true;
	}

	if (job.type == 2)
	{
		// Check if the BI predecessor is active
		for (pair<int, activity> scheduledjob : schedule)
		{
			if (job.predecessors[0] == scheduledjob.second.number && scheduledjob.first + scheduledjob.second.duration + changeovertime > time) return false;
		}
		// Check if the other predecessors have finished.
		for (int predecessor : job.predecessors)
		{
			if (predecessor == job.predecessors[0]) continue;
			bool contains = false;
			for (pair<int, activity> scheduledjob : schedule)
			{
				if (scheduledjob.second.number != predecessor) continue;
				contains = true;
				if (scheduledjob.first + scheduledjob.second.duration > time) return false;
			}
			if (!contains) return false;
		}
		return true;
	}
	return false;
}

// Gets the smallest finish time of a scheduled activity that is greater than the current time
int getNextTime(int time, vector<pair<int, activity>>& schedule)
{
	int nexttime = INT_MAX;
	for (pair<int, activity> scheduledjob : schedule) {
		if (scheduledjob.first + scheduledjob.second.duration <= time) continue;
		nexttime = min(scheduledjob.first + scheduledjob.second.duration, nexttime);
	}

	for (pair<int, activity> scheduledjob : schedule)
	{
		if (scheduledjob.first + scheduledjob.second.duration != nexttime) continue;
		for (int i = 0; i < resourceuse.size(); i++)
		{
			resourceuse[i] -= scheduledjob.second.resourceneeds[i];
		}
	}
	return nexttime;
}

// Check if a schedule satisfies all resource and precedence constraints
bool validSchedule(RCPSP& instance, vector<pair<int, activity>>& schedule)
{
	int makespan = schedule.back().first;
	// Check for each timestep if the resource use is less or equal to the availability
	for (int time = 0; time <= makespan; time++)
	{
		vector<int> resourceusage = vector(resourceuse.size(), 0);
		for (pair<int, activity> scheduledjob : schedule)
		{
			if (time >= scheduledjob.first && time < scheduledjob.first + scheduledjob.second.duration)
			{
				for (int i = 0; i < resourceusage.size(); i++)
				{
					resourceusage[i] += scheduledjob.second.resourceneeds[i];
				}
			}
		}
		for (int i = 0; i < resourceusage.size(); i++)
		{
			if (resourceusage[i] <= instance.resourceavailability[i]) continue;
			return false;
		}
	}
	// Check for each job whether its precedence relations are satisfied
	for (int i = 0; i < instance.jobs; i++)
	{
		if (schedule[i].second.type == 0)
		{
			for (int predecessor : schedule[i].second.predecessors)
			{
				bool contains = false;
				for (int j = 0; j < i; j++)
				{
					if (schedule[j].second.number != predecessor) continue;
					if (schedule[j].first + schedule[j].second.duration <= schedule[i].first) contains = true;
				}
				if (!contains) return false;
			}
		}
		else if (schedule[i].second.type == 1)
		{
			bool check = false;
			for (int predecessor : schedule[i].second.predecessors)
			{
				for (int j = 0; j < i; j++)
				{
					if (schedule[j].second.number == predecessor && schedule[j].first + schedule[j].second.duration <= schedule[i].first) check = true;
				}
			}
			if (!check) return false;
		}
		else if (schedule[i].second.type == 2)
		{
			for (int j = 0; j < instance.jobs; j++)
			{
				if (schedule[j].second.number == schedule[i].second.predecessors[0])
				{
					if (schedule[j].first + schedule[j].second.duration + changeovertime <= schedule[i].first
						|| schedule[i].first + schedule[i].second.duration + changeovertime <= schedule[j].first) continue;
					return false;
				}
			}
			for (int predecessor : schedule[i].second.predecessors)
			{
				if (predecessor == schedule[i].second.predecessors[0]) continue;
				bool contains = false;
				for (int j = 0; j < i; j++)
				{
					if (schedule[j].second.number != predecessor) continue;
					if (schedule[j].first + schedule[j].second.duration <= schedule[i].first) contains = true;
				}
				if (!contains) return false;
			}
		}
	}
	return true;
}

// Print a visual representation of the schedule
void printSchedule(vector<pair<int, activity>>& schedule)
{
	for (pair<int, activity> scheduledjob : schedule)
	{
		std::cout << to_string(scheduledjob.second.number) + "\t" + string(scheduledjob.first, '.') + string(scheduledjob.second.duration, 'X') + string(minmakespan - scheduledjob.first - scheduledjob.second.duration, '.') << std::endl;
	}
}