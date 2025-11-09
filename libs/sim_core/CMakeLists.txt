cmake_minimum_required(VERSION 3.20)

# Core simulation library
add_library(sim_core STATIC
    src/queue/Buffer.cpp
    src/device/Device.cpp
    src/device/DevicePool.cpp
    src/event/Event.cpp
    src/event/EventCalendar.cpp
    src/event/EventDispatcher.cpp
    src/metrics/Metrics.cpp
    src/model/Request.cpp
    src/device/RoundRobinStrategy.cpp
    src/simulator/Simulator.cpp
    src/source/Source.cpp
    src/source/SourcePool.cpp
    src/simulator/ConfigurationManager.cpp
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
