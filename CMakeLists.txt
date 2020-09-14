cmake_minimum_required(VERSION 3.10)

project(raptor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(SRC ./src/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)

add_executable(raptor src/raptor.cpp)
set_target_properties(raptor PROPERTIES LINKER_LANGUAGE CXX)

configure_file(src/raptor_version.h src/raptor_version.h)
target_include_directories(raptor PUBLIC "src")
