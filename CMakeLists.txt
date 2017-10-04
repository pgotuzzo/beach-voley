cmake_minimum_required(VERSION 3.8)
project(beach-voley)

set(CMAKE_CXX_STANDARD 11)

set(LOCK_FILE IPCClasses/LockFile.cpp IPCClasses/LockFile.h)
set(SEMAFORO IPCClasses/Semaforo.cpp IPCClasses/Semaforo.h)
set(PIPE IPCClasses/Pipe.cpp IPCClasses/Pipe.h)
set(MEMORIA_COMPARTIDA IPCClasses/MemoriaCompartida.cpp IPCClasses/MemoriaCompartida.h)

set(LOGGER Logger/Logger.cpp Logger/Logger.h)

set(MONITOR_MAREA src/MonitorMarea.cpp src/MonitorMarea.hpp)

set(SOURCE_FILES main.cpp)
add_executable(beach-voley ${SOURCE_FILES})