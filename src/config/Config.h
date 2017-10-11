#ifndef BEACH_VOLEY_CONFIG_H2
#define BEACH_VOLEY_CONFIG_H2

#include <vector>
#include "ParseException.h"
#include "InvalidConfigException.h"

using namespace std;

enum Mode {
    TOURNAMENT,
    MANUAL
};

struct TournamentParams {
    unsigned int columns;
    unsigned int rows;
    unsigned int capacity;
    unsigned int matches;
    vector<string> players;
    bool debugEnable;
};

struct Config {
    Mode mode;
    TournamentParams tournamentParams;
};

Config parseConfig(vector<string> arguments) throw(ParseException);
void validConfig(Config config) throw(InvalidConfigException);

#endif //BEACH_VOLEY_CONFIG_H2
