#include <iostream>
#include <unistd.h>
#include <wait.h>
#include "config/Config.h"
#include "../util/StringUtils.h"
#include "player/Player.h"
#include "InitException.h"
#include "../IPCClasses/FifoWrite.h"

using namespace std;

/**
 * Shows info about how to use the program
 */
void showHelp() {
    FILE *pHelpFile;
    const int countMax = 100;
    char buffer[countMax];

    pHelpFile = fopen("help", "r");
    if (pHelpFile == nullptr) {
        perror("Error opening help file");
    } else {
        while (!feof(pHelpFile)) {
            char *res = fgets(buffer, countMax, pHelpFile);
            if (res != nullptr) {
                fputs(buffer, stdout);
            }
        }
        fclose(pHelpFile);
    }
}

int main(int argc, char *argv[]) {
    // Creates vector of parameters removing the first one because it is the program's name
    vector<string> vParams;
    for (int i = 1; i < argc; i++) {
        string param(argv[i]);
        vParams.push_back(param);
    }
    Config config;
    try {
        config = parseConfig(vParams);
    } catch (ParseException &e) {
        cout << e.what() << endl;
        return 1;
    }
    cout << "Arguments used are correct. Parsing is complete!" << endl;

    if (config.mode == MANUAL) {
        showHelp();
    } else if (config.mode == TOURNAMENT) {
        cout << "Configuration" << endl
             << "Columns: " << config.tournamentParams.columns << endl
             << "Rows: " << config.tournamentParams.rows << endl
             << "Capacity: " << config.tournamentParams.capacity << endl
             << "Matches count: " << config.tournamentParams.matches << endl
             << "Players: " << toString(config.tournamentParams.players) << endl
             << "Debug: " << (config.tournamentParams.debugEnable ? "true" : "false") << endl;
    }


    return 0;
}