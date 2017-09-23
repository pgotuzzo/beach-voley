#ifndef BEACH_VOLEY_CONFIG_H2
#define BEACH_VOLEY_CONFIG_H2

#include <vector>
#include "ParseException.h"

using namespace std;

enum Mode {
    TOURNAMENT,
    PLAYER,
    MANUAL
};

struct Config {
    Mode mode;
    vector<string> parameters;
    bool debugEnable;
};

Config parseConfig(vector<string> arguments) throw(ParseException);

#endif //BEACH_VOLEY_CONFIG_H2
