cmake_minimum_required(VERSION 3.22)
project(HW1)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(lib)

add_executable(HW1
        lib/Bomb.cpp
        lib/Bomb.h
        lib/Inputs.cpp
        lib/Inputs.h
        lib/Map.cpp
        lib/Map.h
        bgame.cpp
        logging.c
        logging.h
        message.c
        message.h lib/Obstacle.cpp lib/Obstacle.h lib/Bomber.cpp lib/Bomber.h test/driver.cpp)
