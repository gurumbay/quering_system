cmake_minimum_required(VERSION 3.20)

# CLI executable
add_executable(sim_cli 
    src/main.cpp
)

target_link_libraries(sim_cli PRIVATE sim_core)

# Warnings
if(MSVC)
  target_compile_options(sim_cli PRIVATE /W4 /permissive- /EHsc)
else()
  target_compile_options(sim_cli PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_features(sim_cli PUBLIC cxx_std_20)

# Sweep runner: grid search over configurations
add_executable(sim_sweep
  src/sim_sweep.cpp
)

target_link_libraries(sim_sweep PRIVATE sim_core)
