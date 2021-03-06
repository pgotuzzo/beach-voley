#include <iostream>
#include <sys/wait.h>
#include "config/Config.h"
#include "config/Constants.h"
#include "managers/Manager.h"
#include "../IPCClasses/signal/SignalHandler.h"
#include "../IPCClasses/signal/SIGINT_Handler.h"
#include "managers/TournamentBoard.hpp"
#include "monitor/TideMonitor.h"
#include "util/StringUtils.h"

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
    int minCheckTideInMili = 100000;
    int maxCheckTideInMili = 500000;
    int playersToStartTournament = 10;

    Pipe managerReceive;

    VectorCompartido<int> *pointsTable = ResourceHandler::getInstance()->getVectorCompartido(
            SHARED_MEMORY_POINTS_VECTOR);
    LockFile lockForSharedVectors(LOCK_FILE_SHARED_VECTORS);

    Pipe pipesToPlayers[config.tournamentParams.players.size()];
    Semaforo *stadiumTurnstile = ResourceHandler::getInstance()->getSemaforo(SEM_TURNSTILE);
    Semaforo *entranceToFields = ResourceHandler::getInstance()->getSemaforo(SEM_FILE_FIELD_ENTRANCE);
    Semaforo *exitFromField = ResourceHandler::getInstance()->getSemaforo(SEM_FILE_FIELD_EXIT);

    // [C X R] Fields
    vector<vector<int>> fieldsInColumnsPids;
    for (int i = 0; i < config.tournamentParams.columns; i++) {
        vector<int> fieldPidsColumn;
        for (int j = 0; j < config.tournamentParams.rows; j++) {
            __pid_t fieldPid = fork();
            if (fieldPid == 0) {
                Field field{static_cast<unsigned short>(i * config.tournamentParams.rows + j), entranceToFields,
                            exitFromField, &managerReceive, minGameDurationInMili, maxGameDurationInMili};
                field.startPlayingGames();
                exit(0);
            }
            fieldPidsColumn.push_back(fieldPid);
        }
        fieldsInColumnsPids.push_back(fieldPidsColumn);
    }


    __pid_t tideMonitorPid = fork();
    if (tideMonitorPid == 0) {
        TideMonitor tideMonitor{1000 * minCheckTideInMili, 1000 * maxCheckTideInMili, 0, 0, fieldsInColumnsPids};
        tideMonitor.startMonitoring();
        exit(0);
    }

    // Players
    for(int i = 0; i < config.tournamentParams.players.size(); i++) {
        __pid_t playerPid = fork();
        if (playerPid == 0) {
            Player player(i, entranceToFields, exitFromField, stadiumTurnstile, &(pipesToPlayers[i]), &managerReceive);
            player.play();
            exit(0);
        }
    }

    __pid_t managerPid = fork();
    if (managerPid == 0) {
        Manager manager{config.tournamentParams.rows*config.tournamentParams.columns, config.tournamentParams.capacity,
                        config.tournamentParams.matches,
                        static_cast<unsigned int>(config.tournamentParams.players.size()), &lockForSharedVectors,
                        pointsTable, &managerReceive, pipesToPlayers, playersToStartTournament};
        manager.receiveTask();
        exit(0);
    }

    // Interrumption handler
    SIGINT_Handler handler;
    SignalHandler::getInstance()->registrarHandler(SIGINT, (EventHandler *) &handler);

    auto processToWait = static_cast<int>(config.tournamentParams.players.size()
                                          + config.tournamentParams.columns * config.tournamentParams.rows);
    for (int i = 0; i < processToWait; i++) {
        wait(nullptr);
    }
//
//    TournamentBoard tournamentBoard{idsTable, pointsTable, &lockForSharedVectors,
//                                    config.tournamentParams.players.size()};
//    tournamentBoard.printTableValues();

    kill(tideMonitorPid, SIGKILL);
    // 3 = tide monitor, manager, game board
    for (int i = 0; i < 2; i++) {
        wait(nullptr);
    }
    cout << "FINALIZANDO...";

    ResourceHandler::getInstance()->freeResources();
    exit(0);
}

int main(int argc, char *argv[]) {
    Config config = parseArguments(argc, argv);
    validConfig(config);
    int tidePid = 0;
    if (config.mode == MANUAL) {
        showHelp();
        exit(0);
    } else if (config.mode == TOURNAMENT) {
        printConf(config);
        ResourceHandler::init(config);
        playTournament(config);
    }
}