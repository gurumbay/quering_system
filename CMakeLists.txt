cmake_minimum_required(VERSION 3.20)

project(quering_system LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add subdirectories
add_subdirectory(libs/sim_core)
add_subdirectory(apps/cli)
add_subdirectory(apps/gui)
