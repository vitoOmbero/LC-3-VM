# CMakeList.txt : CMake project for Vmc, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(lc3-asm)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "asmc.cpp" )
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME}
	lc3::identifiers
)

# TODO: Add tests and install targets if needed.
