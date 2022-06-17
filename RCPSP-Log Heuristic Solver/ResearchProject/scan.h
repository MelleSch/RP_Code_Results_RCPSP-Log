#ifndef scan_h
#define scan_h
#include <string>
#include "RCPSP.h"


RCPSP readFile(std::string filename);
int readSection(std::istream& infile, int section);
RCPSP initRcpsp(int start, int mod, int bi);
class scan
{
public:
};

#endif