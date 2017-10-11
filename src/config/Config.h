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
    int columns;
    int rows;
    int capacity;
    int matches;
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
