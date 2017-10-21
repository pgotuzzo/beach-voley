cmake_minimum_required(VERSION 3.8)
project(beach_voley)

set(CMAKE_CXX_STANDARD 11)

set(IPC
        src/ipc/Semaforo.cpp src/ipc/Semaforo.h
        src/ipc/Pipe.cpp src/ipc/Pipe.h
        src/ipc/signal/SignalHandler.cpp src/ipc/signal/SignalHandler.h
        src/ipc/signal/EventHandler.h
        src/ipc/signal/SIGINT_Handler.h)

set(UTILS
        src/util/Logger.cpp src/util/Logger.h
        src/util/FileUtils.cpp src/util/FileUtils.h
        src/util/ResourceHandler.cpp src/util/ResourceHandler.h
        src/util/PlayerUtils.cpp src/util/PlayerUtils.h
        src/util/RandomNumber.cpp src/util/RandomNumber.h)

set(CONS src/cons/Path.h src/cons/Definition.h src/cons/Constants.h)

set(MANAGER
        src/manager/SecurityGuard.cpp src/manager/SecurityGuard.h
        src/manager/ManagerProcess.cpp src/manager/ManagerProcess.h src/manager/MatchMaker.cpp src/manager/MatchMaker.h)

set(PLAYER
        src/player/Player.cpp src/player/Player.h
        src/player/PlayerProcess.cpp src/player/PlayerProcess.h)

set(STADIUM
        src/stadium/Stadium.cpp src/stadium/Stadium.h
        src/stadium/Field.cpp src/stadium/Field.h
        src/stadium/FieldProcess.cpp src/stadium/FieldProcess.h)

set(SOURCE_FILES src/main.cpp src/LauncherProcess.cpp src/LauncherProcess.h)
add_executable(beach_voley ${SOURCE_FILES} ${IPC} ${UTILS} ${CONS} ${MANAGER} ${PLAYER} ${STADIUM})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tmp)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/logs)