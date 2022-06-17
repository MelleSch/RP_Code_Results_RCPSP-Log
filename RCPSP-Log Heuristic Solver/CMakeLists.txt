# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("ResearchProject")

# Include sub-projects.
add_subdirectory ("ResearchProject")

set_property (TARGET ResearchProject
  PROPERTY
    # Enable C++17 standard compliance
    CXX_STANDARD 17
)