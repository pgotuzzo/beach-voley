#ifndef BEACH_VOLEY_CONFIG_H2
#define BEACH_VOLEY_CONFIG_H2

#include <vector>
#include "ParseException.h"
#include "InvalidConfigException.h"
#include "../cons/Definition.h"

using namespace std;

Config parseConfig(vector<string> arguments) throw(ParseException);

void validConfig(Config config) throw(InvalidConfigException);

#endif //BEACH_VOLEY_CONFIG_H2
