cmake_minimum_required(VERSION 3.8)
project(beach-voley)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp config/Config.cpp config/ParseException.cpp config/ParseException.h config/Config.h player/Player.cpp player/Player.h Tournament.cpp Tournament.h Constants.h InitException.cpp InitException.h player/PartnerRequester.cpp player/PartnerRequester.h player/Fifo.cpp player/Fifo.h player/FifoWrite.cpp player/FifoWrite.h player/FifoRead.cpp player/FifoRead.h)
add_executable(beach-voley ${SOURCE_FILES})
configure_file(help ${CMAKE_CURRENT_BINARY_DIR}/help COPYONLY)