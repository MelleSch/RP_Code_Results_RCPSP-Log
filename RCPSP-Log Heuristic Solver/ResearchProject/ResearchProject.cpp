// ResearchProject.cpp : Defines the entry point for the application.
//

#include "ResearchProject.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "RCPSP.h"
#include "scan.h"
#include "solve.h"


using std::cout; using std::cin;
using std::endl; using std::string;
using std::filesystem::directory_iterator;

int main() {
    // Get the absolute path to the folder containing problem instances, system dependent
    string path = std::filesystem::current_path().parent_path().parent_path().parent_path().parent_path().string() + "\\ResearchProject\\j30";
    
    // For each file in the directory, print its filename and the makespan of the constructed schedule
    for (const auto& file : directory_iterator(path)) {
        std::cout << file.path().filename().string() << std::endl;
        RCPSP instance = readFile(file.path().string());
            solveInstance(instance);
    }
    
    return EXIT_SUCCESS;
}