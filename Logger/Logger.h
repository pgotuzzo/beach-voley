#ifndef TP1_LOGGER_H
#define TP1_LOGGER_H

#include <iostream>

class Logger {
private:
    const char *fecha;
    static Logger *instance;

    Logger() = default;

public:
    static Logger *getInstance();

    void logMessage(const char *) const;
};


#endif //TP1_LOGGER_H
