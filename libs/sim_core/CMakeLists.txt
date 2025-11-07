cmake_minimum_required(VERSION 3.20)

# Core simulation library
add_library(sim_core STATIC
    src/core/Buffer.cpp
    src/core/Device.cpp
    src/core/Event.cpp
    src/core/EventCalendar.cpp
    src/core/Metrics.cpp
    src/core/Request.cpp
    src/core/Simulator.cpp
    src/core/SimulationConfig.cpp
    src/observers/MetricsObserver.cpp
    src/utils/ConstantDistribution.cpp
    src/utils/ExponentialDistribution.cpp
)

target_include_directories(sim_core PUBLIC include)

# Warnings
if(MSVC)
  target_compile_options(sim_core PRIVATE /W4 /permissive- /EHsc)
else()
  target_compile_options(sim_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_features(sim_core PUBLIC cxx_std_20)
