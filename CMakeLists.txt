cmake_minimum_required(VERSION 3.31)
project(cosmos)

set(CMAKE_CXX_STANDARD 20)

add_executable(cosmo main.cpp
        Astra.h
        Cosmos.cpp
        Cosmos.h
        Astra.cpp
        Particle.cpp
        Particle.h
        random.cpp
        random.h
        mac_rounded_window.mm
        mac_rounded_window.h)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

if (APPLE)
    target_sources(cosmo PRIVATE mac_rounded_window.mm)
    find_library(COCOA_FRAMEWORK Cocoa)
    if (COCOA_FRAMEWORK)
        target_link_libraries(cosmo PRIVATE ${COCOA_FRAMEWORK} ${SDL2_LIBRARIES})
    else ()
        target_link_libraries(cosmo ${SDL2_LIBRARIES})
    endif ()
endif ()
