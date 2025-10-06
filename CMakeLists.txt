cmake_minimum_required(VERSION 3.20)

project(quering_system LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings
if(MSVC)
  add_compile_options(/W4 /permissive- /EHsc)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Main executable
add_executable(sim_cli 
    src/main.cpp
    src/core/Request.cpp
    src/core/Event.cpp
    src/core/Device.cpp
    src/core/Buffer.cpp
    src/core/EventCalendar.cpp
    src/core/Metrics.cpp
    src/core/Simulator.cpp
)
target_include_directories(sim_cli PRIVATE include)
