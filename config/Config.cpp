#include <regex>
#include "Config.h"
#include "../util/StringUtils.h"

using namespace std;

struct Option {
    string name;
    vector<string> params;

    bool operator==(const Option &option) const {
        return option.name == name;
    }
};

const char *REGEX_PATTERN_OPTION = "\\-.*";

// MODE
const char *MAPPER_MODE_MANUAL = "-h";
const int MODES_COUNT = 1;
const char *MODES[MODES_COUNT] = {MAPPER_MODE_MANUAL};

// OPTION
const char *MAPPER_OPT_PLAYER = "-player";
const char *MAPPER_OPT_COLUMNS = "-c";
const char *MAPPER_OPT_ROWS = "-r";
const char *MAPPER_OPT_CAPACITY = "-m";
const char *MAPPER_OPT_MATCHES_COUNT = "-k";
const char *MAPPER_OPT_DEBUG = "-debug";

// TOURNAMENT MODE OPTIONS
const char *TOURNAMENT_OPT_OPTIONAL[1] = {
        MAPPER_OPT_DEBUG
};
const char *TOURNAMENT_OPT_REQUIRED[5] = {
        MAPPER_OPT_PLAYER,
        MAPPER_OPT_COLUMNS,
        MAPPER_OPT_ROWS,
        MAPPER_OPT_CAPACITY,
        MAPPER_OPT_MATCHES_COUNT
};

bool isOption(const string &argument) {
    regex regOpt = regex(REGEX_PATTERN_OPTION);
    return regex_match(argument, regOpt);
}

bool isParam(const string &argument) {
    return !isOption(argument);
}

bool validateOption(const Option &option) {
    if (option.name == MAPPER_OPT_DEBUG) {
        return option.params.empty();
    } else if (option.name == MAPPER_OPT_ROWS ||
               option.name == MAPPER_OPT_COLUMNS ||
               option.name == MAPPER_OPT_MATCHES_COUNT ||
               option.name == MAPPER_OPT_CAPACITY) {
        try {
            stoi(option.params.front());
        } catch (exception &e) {
            return false;
        }
        return option.params.size() == 1;
    } else if (option.name == MAPPER_OPT_PLAYER) {
        return !option.params.empty();
    }
    return false;
}

void setOption(Config *config, Option option) {
    if (option.name == MAPPER_OPT_DEBUG) {
        config->tournamentParams.debugEnable = true;
    } else if (option.name == MAPPER_OPT_ROWS) {
        config->tournamentParams.rows = stoi(option.params.front());
    } else if (option.name == MAPPER_OPT_COLUMNS) {
        config->tournamentParams.columns = stoi(option.params.front());
    } else if (option.name == MAPPER_OPT_CAPACITY) {
        config->tournamentParams.capacity = stoi(option.params.front());
    } else if (option.name == MAPPER_OPT_MATCHES_COUNT) {
        config->tournamentParams.matches = stoi(option.params.front());
    } else if (option.name == MAPPER_OPT_PLAYER) {
        config->tournamentParams.players = option.params;
    }
}

/**
 * If there is any parameters it removes them from {@param arguments}
 *
 * @param arguments
 * @return   all the parameters detected (could be none, in that case an empty vector is returned)
 */
vector<string> getParameters(vector<string> *arguments) {
    vector<string> parameters;
    bool done = arguments->empty();
    while (!done) {
        string arg;
        arg = arguments->front();
        if (isParam(arg)) {
            parameters.push_back(arg);
            arguments->erase(arguments->begin());
        }
        done = !isParam(arg) || arguments->empty();
    }
    return parameters;
}

/**
 * Validates mapping from Option to Mode could be achieved
 * Conditions checked:
 *  - MANUAL doesn't allow parameters associated to it
 *
 * @param option
 * @return  true if it is a valid Mode
 */
bool isMode(Option option) {
    return (option.name == MAPPER_MODE_MANUAL && option.params.empty());
}

/**
 * Parse a Mode by its name
 *
 * @param name
 * @return      Mode parsed
 */
Mode parseMode(const string &name) throw(ParseException) {
    if (name == MAPPER_MODE_MANUAL) {
        return MANUAL;
    } else {
        throw ParseException("Parsing error: " + name + " it is NOT a valid Mode!");
    }
}

Config parseConfig(vector<string> arguments) throw(ParseException) {
    /*
     * Parses arguments to generate a vector of valid Option instances.
     * Expected example: -optionName1 params... -optionName2 params...
     */

    vector<Option> options;
    while (!arguments.empty()) {
        // First element MUST be an option
        string optionName;
        optionName = arguments.front();
        if (!isOption(optionName)) {
            throw ParseException("Argument: " + optionName + " it is NOT a valid option!");
        }
        // Removes first element
        arguments.erase(arguments.begin());
        // Get option's parameters
        vector<string> parameters = getParameters(&arguments);
        // Valid option -> Storing it
        Option option = {optionName, parameters};
        options.push_back(option);
    }

    /*
     * Analise if the generated Option's vector.
     */

    if (!options.empty()) {
        for (auto it = options.begin(); it != options.end(); ++it) {
            // Valid Option or Mode
            if (!isMode(*it) && !validateOption(*it)) {
                // Invalid - ParseException
                throw ParseException(
                        "Unrecognized option:\n\tName: " + it->name + "\n\tParameters: " + toString(it->params)
                );
            }
            // Unique arguments condition
            auto matchPos = find(it + 1, options.end(), *it);
            if (matchPos != options.end()) {
                throw ParseException("Argument: " + matchPos->name + " appears more than once!");
            }
        }
    }

    // Default values
    Config config;
    config.mode = TOURNAMENT;
    TournamentParams tournamentParams;
    tournamentParams.debugEnable = false;
    config.tournamentParams = tournamentParams;

    // Mode
    bool modeFound = false;
    for (auto &option : options) {
        if (isMode(option)) {
            if (modeFound) {
                string error = "Mode are UNIQUE! modes: " + toString(MODES, MODES_COUNT);
                throw ParseException(error);
            } else {
                modeFound = true;
                config.mode = parseMode(option.name);
            }
        }
    }

    // Options for current Mode
    if (config.mode == MANUAL) {
        // Manual
        if (options.size() > 1) {
            throw ParseException("Manual mode doesn't support options");
        }
    } else {
        // Tournament
        //      Required
        for (auto &optionName : TOURNAMENT_OPT_REQUIRED) {
            Option option = {optionName};
            auto match = find(options.begin(), options.end(), option);
            if (match == options.end()) {
                throw ParseException("Missing required option: " + option.name);
            } else {
                setOption(&config, *match);
            }
        }
        //      Optional
        for (auto &optionName : TOURNAMENT_OPT_OPTIONAL) {
            Option option = {optionName};
            auto match = find(options.begin(), options.end(), option);
            if (match != options.end()) {
                setOption(&config, *match);
            }
        }
    }

    return config;
}