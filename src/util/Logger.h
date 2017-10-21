#ifndef BEACH_VOLEY_AUX_LOGGER_H
#define BEACH_VOLEY_AUX_LOGGER_H

#include <iostream>

using namespace std;

class Logger {
private:
    static void log(string message);
public:
    static void reset();
    static void i(string message);
    static void d(string message);
    static void w(string message);
    static void e(string message);
};


#endif //BEACH_VOLEY_AUX_LOGGER_H
