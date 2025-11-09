cmake_minimum_required(VERSION 3.20)

# Enable Qt automoc/uic/rcc for moc-generated code
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt GUI
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# GUI executable
set(GUI_SOURCES
    src/main_qt.cpp
    src/widgets/MainWindow.cpp
    src/widgets/MainWindow.h
    src/widgets/AnalyticsWidget.cpp
    src/widgets/AnalyticsWidget.h
    src/widgets/EventCalendarWidget.cpp
    src/widgets/EventCalendarWidget.h
    src/widgets/TimelineObserver.cpp
    src/widgets/TimelineObserver.h
    src/widgets/TimelineWidget.cpp
    src/widgets/TimelineWidget.h
)

qt_add_executable(sim_gui WIN32 ${GUI_SOURCES})
target_link_libraries(sim_gui PRIVATE sim_core Qt6::Widgets)

# Include directory for widgets (so they can find each other)
target_include_directories(sim_gui PRIVATE src/widgets)

# Warnings
if(MSVC)
  target_compile_options(sim_gui PRIVATE /W4 /permissive- /EHsc)
else()
  target_compile_options(sim_gui PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_features(sim_gui PUBLIC cxx_std_20)
