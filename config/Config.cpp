#include <vector>
#include <regex>
#include "Config.h"

using namespace std;

struct Option {
    string name;
    vector<string> params;

    bool operator==(const Option &option) const {
        return option.name == name;
    }
};

const char *REGEX_PATTERN_OPTION = "\\-.*";

// MODES
const char *MAPPER_MODE_PLAYER = "-player";
const char *MAPPER_MODE_MANUAL = "-h";

// FLAGS
const char *MAPPER_FLAG_DEBUG = "-debug";


bool isOption(const string &argument) {
    regex regOpt = regex(REGEX_PATTERN_OPTION);
    return regex_match(argument, regOpt);
}

bool isParam(const string &argument) {
    return !isOption(argument);
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
 *  - PLAYER requires parameters
 *  - MANUAL doesn't allow parameters associated to it
 *
 * @param option
 * @return  true if it is a valid Mode
 */
bool isMode(Option option) {
    return (option.name == MAPPER_MODE_PLAYER && !option.params.empty()) ||
           (option.name == MAPPER_MODE_MANUAL && option.params.empty());
}

/**
 * Checks if Option matches with DEBUG flag
 *
 * @param option
 * @return  true if matches
 */
bool isDebugFlag(Option option) {
    return option.name == MAPPER_FLAG_DEBUG && option.params.empty();
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
    } else if (name == MAPPER_MODE_PLAYER) {
        return PLAYER;
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
     * Analise if the generated Option's vector. Expected condition
     * - Unique options
     * - One or none Mode can be generated
     */

    // Unique condition
    if (!options.empty()) {
        for (auto it = options.begin(); it != options.end() - 1; ++it) {
            auto matchPos = find(it + 1, options.end(), *it);
            if (matchPos != options.end()) {
                throw ParseException("Argument: " + matchPos->name + " appears more than once!");
            }
        }
    }
    // None or one Mode condition
    bool modeFound = false;
    // Default values
    Mode mode = TOURNAMENT;
    vector<string> modeParams;
    bool enableDebug = false;
    for (auto &option : options) {
        if (isMode(option)) {
            // Mode
            if (modeFound) {
                throw ParseException("Options are UNIQUE! options: [-h -player]");
            } else {
                modeFound = true;
                mode = parseMode(option.name);
                modeParams = option.params;
            }
        } else if (isDebugFlag(option)) {
            // Debug Flag
            enableDebug = true;
        } else {
            // Invalid - ParseException
            string parameters;
            for (auto &param : option.params) {
                parameters += " " + param;
            }
            throw ParseException("Unrecognized option:\n\tName: " + option.name + "\n\tParameters: " + parameters);
        }
    }
    Config config = {mode, modeParams, enableDebug};
    return config;
}