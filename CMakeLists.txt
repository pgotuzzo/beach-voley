cmake_minimum_required(VERSION 3.8)
project(beach-voley)

set(CMAKE_CXX_STANDARD 11)

# IPC
set(LOCK_FILE IPCClasses/LockFile.cpp IPCClasses/LockFile.h)
set(SEMAFORO IPCClasses/Semaforo.cpp IPCClasses/Semaforo.h)
set(PIPE IPCClasses/Pipe.cpp IPCClasses/Pipe.h)
set(MEMORIA_COMPARTIDA IPCClasses/MemoriaCompartida.h)
set(SIGNAL_HANDLER IPCClasses/signal/SignalHandler.h IPCClasses/signal/SignalHandler.cpp IPCClasses/signal/EventHandler.h IPCClasses/signal/SIGINT_Handler.h)
set(FIFO IPCClasses/fifo/FifoRead.cpp IPCClasses/fifo/FifoRead.h IPCClasses/fifo/FifoWrite.cpp IPCClasses/fifo/FifoWrite.h IPCClasses/fifo/Fifo.cpp IPCClasses/fifo/Fifo.h )

# Log
set(LOGGER Logger/Logger.cpp Logger/Logger.h)

# Utils
set(RANDOM_NUMBER util/RandomNumber.cpp util/RandomNumber.hpp)
set(STRING_UTILS util/StringUtils.cpp util/StringUtils.h)

# Exceptions
set(EXCEPTION src/config/ParseException.cpp src/config/ParseException.h src/InitException.cpp src/InitException.h)

# Source
set(TIDE_MONITOR src/monitor/TideMonitor.cpp src/monitor/TideMonitor.hpp)

set(SOURCE_FILES src/. src/main.cpp src/config/Config.cpp src/config/Config.h src/player/Player.cpp src/player/Player.h src/Constants.h util/StringUtils.h util/StringUtils.cpp src/court/Court.cpp src/court/Court.h src/player/PartnerRequester.h src/player/PartnerRequester.cpp src/court/Field.cpp src/court/Field.h util/ResourceHandler.cpp util/ResourceHandler.h)

add_executable(beach-voley
        ${SOURCE_FILES}
        ${LOCK_FILE}
        ${SEMAFORO}
        ${PIPE}
        ${MEMORIA_COMPARTIDA}
        ${FIFO}
        ${SIGNAL_HANDLER}
        ${LOGGER}
        ${RANDOM_NUMBER}
        ${TIDE_MONITOR}
        ${STRING_UTILS}
        ${EXCEPTION})

configure_file(help ${CMAKE_CURRENT_BINARY_DIR}/help COPYONLY)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tmp)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/logs)