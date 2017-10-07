cmake_minimum_required(VERSION 3.8)
project(beach-voley)

set(CMAKE_CXX_STANDARD 11)

set(LOCK_FILE IPCClasses/LockFile.cpp IPCClasses/LockFile.h)
set(SEMAFORO IPCClasses/Semaforo.cpp IPCClasses/Semaforo.h)
set(PIPE IPCClasses/Pipe.cpp IPCClasses/Pipe.h)
set(MEMORIA_COMPARTIDA IPCClasses/MemoriaCompartida.h)

set(LOGGER Logger/Logger.cpp Logger/Logger.h)

set(TIDE_MONITOR src/monitor/TideMonitor.cpp src/monitor/TideMonitor.hpp)

set(RANDOM_NUMBER util/RandomNumber.cpp util/RandomNumber.hpp)
set(STRING_UTILS util/StringUtils.cpp util/StringUtils.h)

set(SOURCE_FILES src/. src/main.cpp src/config/Config.cpp src/config/ParseException.cpp src/config/ParseException.h src/config/Config.h src/player/Player.cpp src/player/Player.h src/Tournament.cpp src/Tournament.h src/Constants.h src/InitException.cpp src/InitException.h util/StringUtils.h util/StringUtils.cpp src/court/Court.cpp src/court/Court.h src/player/PartnerRequester.h src/player/PartnerRequester.cpp IPCClasses/FifoRead.cpp IPCClasses/FifoRead.h IPCClasses/FifoWrite.cpp IPCClasses/FifoWrite.h IPCClasses/Fifo.cpp IPCClasses/Fifo.h src/court/Field.cpp src/court/Field.h)
add_executable(beach-voley ${SOURCE_FILES} ${LOCK_FILE} ${SEMAFORO} ${PIPE} ${MEMORIA_COMPARTIDA} ${LOGGER}
        ${RANDOM_NUMBER} ${TIDE_MONITOR} ${STRING_UTILS})
configure_file(help ${CMAKE_CURRENT_BINARY_DIR}/help COPYONLY)