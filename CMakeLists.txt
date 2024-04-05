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

add_executable(mbp_tagescl_64kb
  src/mbplib_main.cpp
)
set_target_properties(mbp_tagescl_64kb
  PROPERTIES CXX_STANDARD 17 INTERPROCEDURAL_OPTIMIZATION TRUE
)
target_compile_definitions(mbp_tagescl_64kb PRIVATE TAGE_SC_L_SIZE=64)
target_include_directories(mbp_tagescl_64kb PRIVATE include)
target_compile_options(mbp_tagescl_64kb
  PRIVATE "-Wall" "-O3" "-march=native" "-mtune=native"
)
target_link_libraries(mbp_tagescl_64kb PRIVATE mbp_sim mbp_trace_reader)

add_executable(mbp_tagescl_80kb
  src/mbplib_main.cpp
)
set_target_properties(mbp_tagescl_80kb
  PROPERTIES CXX_STANDARD 17 INTERPROCEDURAL_OPTIMIZATION TRUE
)
target_compile_definitions(mbp_tagescl_80kb PRIVATE TAGE_SC_L_SIZE=80)
target_include_directories(mbp_tagescl_80kb PRIVATE include)
target_compile_options(mbp_tagescl_80kb
  PRIVATE "-Wall" "-O3" "-march=native" "-mtune=native"
)
target_link_libraries(mbp_tagescl_80kb PRIVATE mbp_sim mbp_trace_reader)

# function(add_predictor name)
#   add_executable(${name}
#     src/main.cpp
#     src/tage_sc_l.cpp
#     include/tage_sc_l.hpp
#     include/mbplib_compat_layer.hpp
#     include/sbbt_branch_ahead_reader.hpp
#     include/branch_ahead_simulate.hpp
#   )
#   set_target_properties(${name}
#     PROPERTIES CXX_STANDARD 17 INTERPROCEDURAL_OPTIMIZATION TRUE
#   )
#   target_compile_definitions(${name} PRIVATE ${ARGN})
#   target_include_directories(${name} PRIVATE include)
#   target_link_libraries(${name} PRIVATE mbp_sim mbp_trace_reader)
#   target_compile_options(${name}
#     PRIVATE "-Wall" "-O3" "-march=native" "-mtune=native"
#   )
# endfunction()
