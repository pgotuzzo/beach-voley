cmake_minimum_required(VERSION 3.8)
project(beach-voley)

set(CMAKE_CXX_STANDARD 11)

set(LOCK_FILE IPCClasses/LockFile.cpp IPCClasses/LockFile.h)
set(SEMAFORO IPCClasses/Semaforo.cpp IPCClasses/Semaforo.h)
set(PIPE IPCClasses/Pipe.cpp IPCClasses/Pipe.h)
set(MEMORIA_COMPARTIDA IPCClasses/MemoriaCompartida.cpp IPCClasses/MemoriaCompartida.h)
set(LOGGER Logger/Logger.cpp Logger/Logger.h)
set(MONITOR_MAREA src/MonitorMarea.cpp src/MonitorMarea.hpp)

set(SOURCE_FILES src/main.cpp src/config/Config.cpp src/config/ParseException.cpp src/config/ParseException.h src/config/Config.h src/player/Player.cpp src/player/Player.h src/Tournament.cpp src/Tournament.h src/Constants.h src/InitException.cpp src/InitException.h util/StringUtils.h util/StringUtils.cpp)
add_executable(beach-voley ${SOURCE_FILES})
configure_file(help ${CMAKE_CURRENT_BINARY_DIR}/help COPYONLY)