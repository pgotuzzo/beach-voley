cmake_minimum_required(VERSION 3.8)
project(beach-voley)

set(CMAKE_CXX_STANDARD 11)

# Log
set(LOGGER src/logger/Logger.cpp src/logger/Logger.h)

# IPC
set(LOCK_FILE IPCClasses/LockFile.cpp IPCClasses/LockFile.h)
set(SEMAFORO IPCClasses/Semaforo.cpp IPCClasses/Semaforo.h)
set(PIPE IPCClasses/Pipe.cpp IPCClasses/Pipe.h)
set(MEMORIA_COMPARTIDA IPCClasses/MemoriaCompartida.h)
set(VECTOR_COMPARTIDO IPCClasses/VectorCompartido.h)

# Utils
set(RANDOM_NUMBER src/util/RandomNumber.cpp src/util/RandomNumber.h)
set(STRING_UTILS src/util/StringUtils.cpp src/util/StringUtils.h)
set(RESOURCE_HANDLER src/util/ResourceHandler.cpp src/util/ResourceHandler.h ${SEMAFORO})

# Signals
set(SIGNAL_HANDLER IPCClasses/signal/SignalHandler.h IPCClasses/signal/SignalHandler.cpp
        IPCClasses/signal/EventHandler.h)
set(SIGINT_HANDLER IPCClasses/signal/SIGINT_Handler.h ${RESOURCE_HANDLER})
set(SIGNAL_HANDLER_FOR_FIELD IPCClasses/signal/SIGINT_HandlerForField.h)

# Exceptions
set(PARSE_EXCEPTION src/config/ParseException.cpp src/config/ParseException.h)
set(INIT_EXCEPTION src/InitException.cpp src/InitException.h)

# Source
set(TIDE_MONITOR src/monitor/TideMonitor.cpp src/monitor/TideMonitor.h ${RANDOM_NUMBER} ${LOGGER})
set(PLAYER src/player/Player.cpp src/player/Player.h ${PIPE}
        src/player/PartnerRequester.h src/player/PartnerRequester.cpp)
set(FIELD src/stadium/Field.cpp src/stadium/Field.h ${SEMAFORO} ${LOGGER} ${RANDOM_NUMBER})
set(STADIUM src/stadium/Stadium.cpp src/stadium/Stadium.h)
set(MANAGER src/managers/Manager.cpp src/managers/Manager.h ${RESOURCE_HANDLER} ${VECTOR_COMPARTIDO})
set(TOURNAMENT_BOARD src/managers/TournamentBoard.cpp src/managers/TournamentBoard.hpp)

# Configs
set(CONFIG src/config/Config.cpp src/config/Config.h ${PARSE_EXCEPTION})

# Main
set(SOURCE_FILES src/main.cpp src/config/Constants.h ${CONFIG}
        ${FIELD} ${PLAYER} ${STADIUM} ${MANAGER} ${TOURNAMENT_BOARD}
        ${STRING_UTILS} ${RESOURCE_HANDLER} ${LOCK_FILE} ${TIDE_MONITOR}
        ${SIGNAL_HANDLER} ${SIGNAL_HANDLER} src/config/InvalidConfigException.cpp src/config/InvalidConfigException.h)
add_executable(beach-voley ${SOURCE_FILES})

# Test
set(TEST_CATCH tests/catch.hpp)
set(TIDE_MONITOR_TEST tests/monitor/TideMonitorTest.cpp ${TEST_CATCH} ${TIDE_MONITOR} ${SIGNAL_HANDLER}
        ${SIGINT_HANDLER})
add_executable(beach-voley-tests ${TIDE_MONITOR_TEST})

configure_file(help ${CMAKE_CURRENT_BINARY_DIR}/help COPYONLY)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tmp)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/logs)