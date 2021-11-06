# CMakeList.txt : CMake project for LC-3, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (LC-3 "LC-3.cpp" "LC-3.h")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

# TODO: Add tests and install targets if needed.
