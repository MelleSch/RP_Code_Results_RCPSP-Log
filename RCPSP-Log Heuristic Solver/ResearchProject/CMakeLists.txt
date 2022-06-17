# CMakeList.txt : CMake project for ResearchProject, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (ResearchProject "ResearchProject.cpp" "ResearchProject.h" "activity.cpp" "activity.h" "RCPSP.cpp" "RCPSP.h" "scan.cpp" "scan.h"  "resource.h" "solve.cpp" "solve.h")




# TODO: Add tests and install targets if needed.
