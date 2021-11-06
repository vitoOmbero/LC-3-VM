# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("LC-3")

# Include sub-projects.
add_subdirectory ("LC-3")
add_subdirectory ("Identifiers")
add_subdirectory ("Asm")
