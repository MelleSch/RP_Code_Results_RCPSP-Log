#include "scan.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "RCPSP.h"
#include "activity.h"
#include <algorithm>

std::string separator = "************************************************************************";
int jobs;
int horizon;
int numrenewable;
std::vector<std::vector<int>> successors;
std::vector<int> durations;
std::vector<std::vector<int>> resourcereqs;
std::vector<int> resourceavailability;

RCPSP readFile(std::string filename)
{
	int section = 0;
	std::ifstream infile(filename);
	std::string line;

	successors.clear();
	durations.clear();
	resourcereqs.clear();
	resourceavailability.clear();
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		if (!line.compare(separator)) { section++; readSection(infile, section); }
		
	}
	// Provide initRcpsp with the arguments kappa_1 kappa_2 and bi which needs to be 0 for OR 1 for BI. If you want a 0 log then use kappa_2=1000
	RCPSP test = initRcpsp(1, 10, 1);
	return test;
}

int readSection(std::istream &infile, int section)
{
	std::string line;
	std::istringstream iss;
	switch (section) {
		case 1:
		{
			infile.ignore(LONG_MAX, '\n');
			infile.ignore(LONG_MAX, '\n');
			break;
		}
		case 2:
		{
			infile.ignore(LONG_MAX, '\n');

			std::vector<std::string> result;
			std::getline(infile, line);
			iss.str(line);
			for (std::string s; iss >> s; )
			{
				result.push_back(s);
			}
			jobs = std::stoi(result.back());
			iss.clear();
			result.clear();

			std::getline(infile, line);
			iss.str(line);
			for (std::string s; iss >> s; )
			{
				result.push_back(s);
			}
			horizon = std::stoi(result.back());
			iss.clear();
			result.clear();

			infile.ignore(LONG_MAX, '\n');

			std::getline(infile, line);
			iss.str(line);
			for (std::string s; iss >> s; )
			{
				result.push_back(s);
			}
			numrenewable = std::stoi(result[3]);
			iss.clear();
			result.clear();

			infile.ignore(LONG_MAX, '\n');
			infile.ignore(LONG_MAX, '\n');

			break;
		}
		case 3:
		{
			infile.ignore(LONG_MAX, '\n');
			infile.ignore(LONG_MAX, '\n');
			infile.ignore(LONG_MAX, '\n');
			break;
		}
		case 4:
		{
			infile.ignore(LONG_MAX, '\n');
			infile.ignore(LONG_MAX, '\n');
			int job;
			int numsuccessors;
			for (int i = 0; i < jobs; i++)
			{
				std::vector<int> currsuccessors;
				std::vector<std::string> result;
				std::getline(infile, line);
				iss.str(line);
				for (std::string s; iss >> s; )
				{
					result.push_back(s);
				}
				job = std::stoi(result[0]);
				numsuccessors = std::stoi(result[2]);
				for (int j = 0; j < numsuccessors; j++)
				{
					currsuccessors.push_back(std::stoi(result[3 + j]));
				}
				successors.push_back(currsuccessors);
				iss.clear();
				result.clear();
			}
			break;
		}
		case 5:
		{
			infile.ignore(LONG_MAX, '\n');
			infile.ignore(LONG_MAX, '\n');
			infile.ignore(LONG_MAX, '\n');
			for (int i = 0; i < jobs; i++)
			{
				std::vector<int> currresourceneeds;
				std::vector<std::string> result;
				std::getline(infile, line);
				iss.str(line);
				for (std::string s; iss >> s; )
				{
					result.push_back(s);
				}
				durations.push_back(std::stoi(result[2]));
				for (int j = 0; j < numrenewable; j++)
				{
					currresourceneeds.push_back(std::stoi(result[3 + j]));
				}
				resourcereqs.push_back(currresourceneeds);
				iss.clear();
				result.clear();
			}
			break;
		}
		case 6:
		{
			infile.ignore(LONG_MAX, '\n');
			infile.ignore(LONG_MAX, '\n');
			std::vector<std::string> result;
			std::getline(infile, line);
			iss.str(line);
			for (std::string s; iss >> s; )
			{
				resourceavailability.push_back(std::stoi(s));
			}
			break;
		}
		case 7:
			break;
	}
	return 0;
}

RCPSP initRcpsp(int start, int mod, int bi)
{
	std::vector<activity> activities;
	for (int i = 1; i <= jobs; i++) {
		int type = ((start + i - 1) % mod) != 0 ? 0 : (1 + bi);
		if (i == 1 || i == jobs) type = 0;
		activity newact = activity(i, durations[i-1], resourcereqs[i-1], successors[i-1], type);
		for (int j = 1; j < i; j++) {
			if (std::binary_search(activities[j-1].successors.begin(), activities[j-1].successors.end(), i))
			{
				newact.addPredecessor(j);
			}
		}
		activities.push_back(newact);
	}
	return RCPSP(jobs, horizon, numrenewable, activities, resourceavailability);
}