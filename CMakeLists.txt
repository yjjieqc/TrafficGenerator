cmake_minimum_required(VERSION 3.5)
project(TrafficGenerator)

set(CMAKE_C_STANDARD 11)

SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_C_FLAGS "-Wall ")

#SET(CMAKE_EXE_LINKER_FLAGS " -lm -lrt -pthread")
#SET(CMAKE_C_FLAGS  "-c -Wall -pthread -lm -lrt"
#SET(CMAKE_C_FLAGS  "-c -Wall ")
#SET(CMAKE_EXE_LINKER_FLAGS "-pthread -lm -lrt")
#SET(CMAKE_C_FLAGS "-pthread -lm -lrt")

ADD_SUBDIRECTORY(src)







