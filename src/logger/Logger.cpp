#include "Logger.h"
#include <fstream>

static const char archivo[] = "logs/log.txt";

Logger *Logger::instance = nullptr;

Logger *Logger::getInstance() {
    if (instance == nullptr) {
        instance = new Logger();
    }
    return instance;
}

void Logger::logMessage(const char *mensaje) const {
    std::ofstream salida(archivo, std::ios::out | std::ios::app);
    salida << mensaje << std::endl;
    salida.close();
}
