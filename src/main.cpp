#include <iostream>
#include <sys/wait.h>
#include "config/Config.h"
#include "config/Constants.h"
#include "managers/Manager.h"
#include "../util/StringUtils.h"
#include "../IPCClasses/signal/SignalHandler.h"
#include "../IPCClasses/signal/SIGINT_Handler.h"
#include "managers/TournamentBoard.hpp"

void playTournament(Config config);

using namespace std;

/**
 * Parses the arguments to the simulation from arg and argv
 *
 * @param argc the number of arguments
 * @param argv the array with the arguments
 * @return a config for the simulation.
 */
Config parseArguments(int argc, char *argv[]) {
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

    Pipe managerReceive;

    VectorCompartido<int> *idsTable = ResourceHandler::getInstance()->getVectorCompartido(SHARED_MEMORY_IDS_VECTOR);
    VectorCompartido<int> *pointsTable = ResourceHandler::getInstance()->getVectorCompartido(
            SHARED_MEMORY_POINTS_VECTOR);
    LockFile lockForSharedVectors(LOCK_FILE_SHARED_VECTORS);

    Pipe pipesToPlayers[config.tournamentParams.players.size()];
    map<int, Pipe *> playersIdPipeMap;
    for (int i = 0; i < config.tournamentParams.players.size(); i++) {
        playersIdPipeMap[i] = &(pipesToPlayers[i]);
    }

    // Manager
    Manager manager{config.tournamentParams, idsTable, pointsTable, &lockForSharedVectors, &managerReceive,
                    playersIdPipeMap};
    manager.initManager();
    TournamentBoard tournamentBoard{idsTable, pointsTable, &lockForSharedVectors};

    // Stadium = [C X R] Fields
    Stadium stadium(config.tournamentParams.columns, config.tournamentParams.rows,
                    1000 * minGameDurationInMili, 1000 * maxGameDurationInMili, &managerReceive);
    stadium.initStadium();

    // Players
    Semaforo *stadiumTurnstile = ResourceHandler::getInstance()->getSemaforo(SEM_TURNSTILE);
    for (int i = 0; i < config.tournamentParams.players.size(); i++) {
        string name = config.tournamentParams.players[i];
        Player player(i, name, &stadium, stadiumTurnstile, &pipesToPlayers[i], &managerReceive);
        player.initPlayer();
    }
    managerReceive.cerrar();
}

int main(int argc, char *argv[]) {
    Config config = parseArguments(argc, argv);
    validConfig(config);

    if (config.mode == MANUAL) {
        showHelp();
        exit(0);
    } else if (config.mode == TOURNAMENT) {
        printConf(config);
        ResourceHandler::init(config);
        playTournament(config);
    }

    // Interrumption handler
    SIGINT_Handler handler;
    SignalHandler::getInstance()->registrarHandler(SIGINT, (EventHandler *) &handler);

    // 3 = tide monitor, manager, game board
    auto processToWait = static_cast<int>(config.tournamentParams.players.size()
                                          + config.tournamentParams.columns * config.tournamentParams.rows + 3);
    for (int i = 0; i < processToWait; i++) {
        wait(nullptr);
    }

    // TODO: create the TideMonitor

    cout << "FINALIZANDO...";

    ResourceHandler::getInstance()->freeResources();
    exit(0);
}