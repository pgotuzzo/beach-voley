cmake_minimum_required(VERSION 3.8)
project(beach-voley)

set(CMAKE_CXX_STANDARD 11)

set(LOCK_FILE IPCClasses/LockFile.cpp IPCClasses/LockFile.h)

set(SOURCE_FILES main.cpp)
add_executable(beach-voley ${SOURCE_FILES})