# How do I compile the code?
The project is set up to be compiled using make, this can be done on both Linux or in Windows Visual Studio using CMake support.

# How do I run different instances?
The main file is ResearchProjec/ResearchProject.cpp, this file requires the path to the problem instance folder, which is system dependent.
The path is set on line 20: "string path = "ABS\\PATH\\TO\\YOUR\\REPOSITORY\\DIRECTORY" + "\\ResearchProject\\j30";"
Then the varying degrees of added logical constraints can be set in scan.cpp on line 38, in the call to initRcpsp(kappa_1, kappa_2, bi). As described in the paper, kappa_1 determines which activities get transformed, kappa_2 determines what percentage of activities get transformed using modulo (1 = 100%, 2 = 50%, 5 = 20%, 10 = 10%, 1000 = 0%), bi determines whether the added constraints are OR or BI constraints (0 = OR, 1 = BI).