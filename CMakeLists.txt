cmake_minimum_required(VERSION 3.8)
project(beach-voley)

set(CMAKE_CXX_STANDARD 11)

# IPC
set(LOCK_FILE IPCClasses/LockFile.cpp IPCClasses/LockFile.h)
set(SEMAFORO IPCClasses/Semaforo.cpp IPCClasses/Semaforo.h)
set(PIPE IPCClasses/Pipe.cpp IPCClasses/Pipe.h)
set(MEMORIA_COMPARTIDA IPCClasses/MemoriaCompartida.h)
set(SIGNAL_HANDLER IPCClasses/signal/SignalHandler.h IPCClasses/signal/SignalHandler.cpp
        IPCClasses/signal/EventHandler.h IPCClasses/signal/SIGINT_Handler.h)
set(FIFO IPCClasses/fifo/FifoRead.cpp IPCClasses/fifo/FifoRead.h
        IPCClasses/fifo/FifoWrite.cpp IPCClasses/fifo/FifoWrite.h IPCClasses/fifo/Fifo.cpp IPCClasses/fifo/Fifo.h )

# Log
set(LOGGER Logger/Logger.cpp Logger/Logger.h)

# Utils
set(RANDOM_NUMBER util/RandomNumber.cpp util/RandomNumber.h)
set(STRING_UTILS util/StringUtils.cpp util/StringUtils.h)
set(RESOURCE_HANDLER util/ResourceHandler.cpp util/ResourceHandler.h)

# Exceptions
set(PARSE_EXCEPTION src/config/ParseException.cpp src/config/ParseException.h)
set(INIT_EXCEPTION src/InitException.cpp src/InitException.h)

# Source
set(TIDE_MONITOR src/monitor/TideMonitor.cpp src/monitor/TideMonitor.h ${RANDOM_NUMBER})
set(PLAYER src/player/Player.cpp src/player/Player.h ${FIFO}
        src/player/PartnerRequester.h src/player/PartnerRequester.cpp)
set(FIELD src/stadium/Field.cpp src/stadium/Field.h ${SEMAFORO} ${LOGGER} ${RANDOM_NUMBER})
set(STADIUM src/stadium/Stadium.cpp src/stadium/Stadium.h)
set(MANAGER  src/manager/Manager.cpp src/manager/Manager.h ${RESOURCE_HANDLER})

# Configs
set(CONFIG src/config/Config.cpp src/config/Config.h ${PARSE_EXCEPTION})

# Main
set(SOURCE_FILES src/. src/main.cpp src/config/Constants.h ${CONFIG}
        ${FIELD} ${PLAYER} ${STADIUM} ${MANAGER}
        ${STRING_UTILS} ${RESOURCE_HANDLER} ${LOCK_FILE}
        ${SIGNAL_HANDLER} ${SIGNAL_HANDLER})
add_executable(beach-voley ${SOURCE_FILES})

configure_file(help ${CMAKE_CURRENT_BINARY_DIR}/help COPYONLY)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tmp)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/logs)