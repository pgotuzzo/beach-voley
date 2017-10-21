#include "Logger.h"
#include "../cons/Path.h"
#include <fstream>

void Logger::log(string message) {
    std::ofstream salida(PATH_LOG, std::ios::out | std::ios::app);
    salida << message << std::endl;
    salida.close();
}

void Logger::i(string message) {
    log("INFO:\t" + message);
}

void Logger::d(string message) {
    log("DEBUG:\t" + message);
}

void Logger::w(string message) {
    log("WARN:\t" + message);
}

void Logger::e(string message) {
    log("ERROR:\t" + message);
}

void Logger::reset() {
    remove(PATH_LOG);
}
