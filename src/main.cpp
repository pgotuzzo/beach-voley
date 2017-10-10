#include <iostream>
#include "config/Config.h"
#include "../util/StringUtils.h"
#include "stadium/Stadium.h"
#include "player/Player.h"
#include "../IPCClasses/signal/SignalHandler.h"
#include "../IPCClasses/signal/SIGINT_Handler.h"
#include "config/Constants.h"
#include "manager/Manager.h"

void playTournament(Config config);

using namespace std;

bool initPlayers(Player player) {
    int pid = fork();
    if (pid == 0) {
        player.init();
        player.play();
        exit(0);
    }
    return true;
}

/**
 * Parses the arguments to the simulation from arg and argv
 *
 * @param argc the number of arguments
 * @param argv the array with the arguments
 * @return a config for the simulation.
 */
Config parseArguments(int argc, char *argv[]){
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
        exit(1);
    }
    cout << "Arguments used are correct. Parsing is complete!" << endl;

    return config;
}

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

/**
 * Prints out the current configuration.
 *
 * @param config the current configuration.
 */
void printConf(Config config) {
    cout << "CONFIGURATION:" << endl
         << "Columns: " << config.tournamentParams.columns << endl
         << "Rows: " << config.tournamentParams.rows << endl
         << "Capacity: " << config.tournamentParams.capacity << endl
         << "Matches count: " << config.tournamentParams.matches << endl
         << "Players: " << toString(config.tournamentParams.players) << endl
         << "Debug: " << (config.tournamentParams.debugEnable ? "true" : "false") << endl;
}

/**
 * Runs the tournament simulation.
 *
 * @param config the configuration for the simulation.
 */
void playTournament(Config config) {
    //TODO: get this from config
    int minGameDurationInMili = 100;
    int maxGameDurationInMili = 500;

    // Stadium = [C X R] Fields
    Stadium stadium(config.tournamentParams.columns, config.tournamentParams.rows,
                    1000 * minGameDurationInMili, 1000 * maxGameDurationInMili);
    stadium.initStadium();

    // Manager
    Manager manager;
    manager.initManager();

    // Players
    Semaforo *stadiumTurnstile = ResourceHandler::getInstance()->createSemaforo(
            SEM_TURNSTILE, 0, config.tournamentParams.capacity
    );
    for (const auto &name : config.tournamentParams.players) {
        Player player(name, &stadium, stadiumTurnstile);
        initPlayers(player);
    }

}

int main(int argc, char *argv[]) {
    Config config = parseArguments(argc, argv);

    if (config.mode == MANUAL) {
        showHelp();
        exit(0);
    } else if (config.mode == TOURNAMENT) {
        printConf(config);
        playTournament(config);
    }

    // Interrumption handler
    SIGINT_Handler handler;
    SignalHandler::getInstance()->registrarHandler(SIGINT, (EventHandler *) &handler);

    for (int i = 0; i < 20; i++) {
        wait(nullptr);
    }

    cout << "FINALIZANDO...";

    exit(0);
}