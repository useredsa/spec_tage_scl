FetchContent_Declare(MBPlib
  GIT_REPOSITORY https://github.com/useredsa/MBPlib.git
  GIT_TAG        fa9a532
)
FetchContent_MakeAvailable(MBPlib)

function(add_test_compile_options target)
  set_target_properties(${target}
    PROPERTIES CXX_STANDARD 17 INTERPROCEDURAL_OPTIMIZATION TRUE
  )
  target_include_directories(${target} PRIVATE ../../include)
  target_compile_options(${target}
    PRIVATE "-Wall" "-O3" "-march=native" "-mtune=native"
  )
endfunction()

add_executable(mbp_tagescl_64kb mbplib_sim_main.cpp)
add_test_compile_options(mbp_tagescl_64kb)
target_compile_definitions(mbp_tagescl_64kb PRIVATE TAGE_SC_L_SIZE=64)
target_link_libraries(mbp_tagescl_64kb
  PRIVATE mbp_sim mbp_trace_reader)

add_executable(mbp_tagescl_80kb mbplib_sim_main.cpp)
add_test_compile_options(mbp_tagescl_80kb)
target_compile_definitions(mbp_tagescl_80kb PRIVATE TAGE_SC_L_SIZE=80)
target_link_libraries(mbp_tagescl_80kb
  PRIVATE mbp_sim mbp_trace_reader)

foreach(cpi RANGE 0 1000 10)
  add_executable(wp_${cpi}_tagescl_64kb wrong_path_sim.cpp)
  add_test_compile_options(wp_${cpi}_tagescl_64kb)
  target_compile_definitions(wp_${cpi}_tagescl_64kb
    PRIVATE
      TAGE_SC_L_SIZE=64
      NUM_CORRECT_PATH_INSTRS=${cpi}
      NUM_WRONG_PATH_BRANCHES=0)
  target_link_libraries(wp_${cpi}_tagescl_64kb
    PRIVATE mbp_sim mbp_trace_reader)
endforeach()

foreach(cpi IN ITEMS 0 50)
  foreach(wpb IN ITEMS 10)
    add_executable(wp_${cpi}_${wpb}_tagescl_64kb wrong_path_sim.cpp)
    add_test_compile_options(wp_${cpi}_${wpb}_tagescl_64kb)
    target_compile_definitions(wp_${cpi}_${wpb}_tagescl_64kb
      PRIVATE
        TAGE_SC_L_SIZE=64
        NUM_CORRECT_PATH_INSTRS=${cpi}
        NUM_WRONG_PATH_BRANCHES=${wpb})
    target_link_libraries(wp_${cpi}_${wpb}_tagescl_64kb
      PRIVATE mbp_sim mbp_trace_reader)
  endforeach()
endforeach()
