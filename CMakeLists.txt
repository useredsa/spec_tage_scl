cmake_minimum_required(VERSION 3.10)

project(SpeculativeTageScl
  DESCRIPTION "An speculative implementation of the TAGE-SC-L branch predictor with adapters for different architecture simulators."
  VERSION 0.1.0
  LANGUAGES CXX
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

Include(FetchContent)

add_subdirectory(test/sbbt/)
