cmake_minimum_required(VERSION 3.31)
project(cosmos)

set(CMAKE_CXX_STANDARD 20)

add_executable(cosmo main.cpp
        Astra.h
        Cosmos.cpp
        Cosmos.h
        Astra.cpp
        Particule.cpp
        Particule.h
        random.cpp
        random.h)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(cosmo ${SDL2_LIBRARIES})
