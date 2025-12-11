cmake_minimum_required(VERSION 3.19)
project(omp-easing-functions LANGUAGES C CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(deps/omp-sdk)

include_directories(
	deps
	deps/pawn/source
	deps/pawn/source/linux
	deps/pawn-natives
)

add_definitions(
	-DHAVE_STDINT_H=1
	-DPAWN_CELL_SIZE=32
)

add_library(${PROJECT_NAME} SHARED main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE OMP-SDK)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
