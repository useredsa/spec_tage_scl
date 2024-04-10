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

FetchContent_Declare(MBPlib
  GIT_REPOSITORY https://github.com/useredsa/MBPlib.git
  GIT_TAG        fa9a532
)
FetchContent_MakeAvailable(MBPlib)

function(add_test_compile_options target)
  set_target_properties(${target}
    PROPERTIES CXX_STANDARD 17 INTERPROCEDURAL_OPTIMIZATION TRUE
  )
  target_include_directories(${target} PRIVATE include)
  target_compile_options(${target}
    PRIVATE "-Wall" "-O3" "-march=native" "-mtune=native"
  )
endfunction()

add_executable(mbp_tagescl_64kb src/mbplib_main.cpp)
add_test_compile_options(mbp_tagescl_64kb)
target_compile_definitions(mbp_tagescl_64kb PRIVATE TAGE_SC_L_SIZE=64)
target_link_libraries(mbp_tagescl_64kb PRIVATE mbp_sim mbp_trace_reader)

add_executable(mbp_tagescl_80kb src/mbplib_main.cpp)
add_test_compile_options(mbp_tagescl_80kb)
target_compile_definitions(mbp_tagescl_80kb PRIVATE TAGE_SC_L_SIZE=80)
target_link_libraries(mbp_tagescl_80kb PRIVATE mbp_sim mbp_trace_reader)

add_executable(test_spec_tagescl_64kb src/random_speculative_sbbt_sim.cpp)
add_test_compile_options(test_spec_tagescl_64kb)
target_compile_definitions(test_spec_tagescl_64kb
  PRIVATE MAX_WRONG_PATH_BRANCHES=10 TAGE_SC_L_SIZE=64)
target_link_libraries(test_spec_tagescl_64kb PRIVATE mbp_sim mbp_trace_reader)

add_executable(test_spec_tagescl_80kb src/random_speculative_sbbt_sim.cpp)
add_test_compile_options(test_spec_tagescl_80kb)
target_compile_definitions(test_spec_tagescl_80kb
  PRIVATE MAX_WRONG_PATH_BRANCHES=10 TAGE_SC_L_SIZE=80)
target_link_libraries(test_spec_tagescl_80kb PRIVATE mbp_sim mbp_trace_reader)
