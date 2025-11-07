cmake_minimum_required(VERSION 3.20)

project(quering_system LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable Qt automoc/uic/rcc for moc-generated code
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt GUI
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Warnings
if(MSVC)
  add_compile_options(/W4 /permissive- /EHsc)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core library
add_library(sim_core STATIC
    src/core/Request.cpp
    src/core/Event.cpp
    src/core/Device.cpp
    src/core/Buffer.cpp
    src/core/EventCalendar.cpp
    src/core/Metrics.cpp
    src/core/Simulator.cpp
    src/core/SimulationConfig.cpp
    src/utils/ConstantDistribution.cpp
    src/utils/ExponentialDistribution.cpp
    src/observers/MetricsObserver.cpp
)
target_include_directories(sim_core PUBLIC include)

# CLI executable
add_executable(sim_cli 
    src/main.cpp
)
target_link_libraries(sim_cli PRIVATE sim_core)

# GUI executable
set(GUI_SOURCES
    src/gui/main_qt.cpp
    src/gui/MainWindow.cpp
    src/gui/MainWindow.h
    src/gui/AnalyticsWidget.cpp
    src/gui/AnalyticsWidget.h
    src/gui/EventCalendarWidget.cpp
    src/gui/EventCalendarWidget.h
    src/gui/TimelineWidget.cpp
    src/gui/TimelineWidget.h
)
qt_add_executable(sim_gui WIN32 ${GUI_SOURCES})
target_link_libraries(sim_gui PRIVATE sim_core Qt6::Widgets)
