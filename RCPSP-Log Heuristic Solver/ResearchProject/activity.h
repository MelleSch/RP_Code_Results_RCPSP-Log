#ifndef activity_h
#define activity_h
#include <vector>

class activity {



public:	
	int number;
	int duration;
	std::vector<int> resourceneeds;
	std::vector<int> predecessors;
	std::vector<int> successors;
	int type;
	int heur;
	activity(int number, int duration, std::vector<int> resourceneeds, std::vector<int> successors, int type);
	void addPredecessor(int predecessor);
	bool equals(activity &other);
};
//
//class andactivity : public activity {
//
//};
//
//class oractivity : public activity {
//
//};
//
//class biactivity : public activity {
//
//};
#endif