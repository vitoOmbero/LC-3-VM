# CMakeList.txt : CMake project for LC-3, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.9)
project(LC-3)

include_directories("private")

set(PRIVATE_SRC 
	"private/memory.cpp"
)

set(ALL_SRC ${PRIVATE_SRC} 
	"LC-3.cpp"
	"LC-3.h"
	"cli.cpp"
)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${ALL_SRC})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME}
	lc3::identifiers
)
# TODO: Add tests and install targets if needed.