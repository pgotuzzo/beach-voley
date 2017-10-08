#include <iostream>
#include "config/Config.h"
#include "../util/StringUtils.h"
#include "stadium/Stadium.h"
#include "player/Player.h"
#include "../IPCClasses/signal/SignalHandler.h"
#include "../IPCClasses/signal/SIGINT_Handler.h"
#include "config/Constants.h"
#include "manager/Manager.h"
#include <sys/wait.h>

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

bool initStadium(Stadium stadium) {
    for (int i = 0; i < stadium.getColumns(); i++) {
        for (int j = 0; j < stadium.getRows(); j++) {
            Field field = stadium.getField(i, j);
            int pid = fork();
            if (pid == 0) {
                field.waitForPlayers();
                return false;
            }
        }
    }
    return true;
}

bool initManager(Manager manager) {
    int pid = fork();
    if (pid == 0) {
        manager.receiveTask();
        return false;
    }
    return true;
}

bool initPlayers(Player player) {
    int pid = fork();
    if (pid == 0) {
        player.play();
        return false;
    }
    return true;
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
        cout << "CONFIGURATION:" << endl
             << "Columns: " << config.tournamentParams.columns << endl
             << "Rows: " << config.tournamentParams.rows << endl
             << "Capacity: " << config.tournamentParams.capacity << endl
             << "Matches count: " << config.tournamentParams.matches << endl
             << "Players: " << toString(config.tournamentParams.players) << endl
             << "Debug: " << (config.tournamentParams.debugEnable ? "true" : "false") << endl;

        // Stadium = [C X R] Fields
        Stadium stadium(config.tournamentParams.columns, config.tournamentParams.rows);
        bool isRoot = initStadium(stadium);
        if (!isRoot) {
            // Tournament ended
            exit(0);
        }

        // Manager
        Manager manager;
        isRoot = initManager(manager);
        if (!isRoot) {
            // Tournament ended
            exit(0);
        }

        // Players
        Semaforo *stadiumTurnstile = ResourceHandler::getInstance()->createSemaforo(
                SEM_TURNSTILE, 0, config.tournamentParams.capacity
        );
        for (const auto &name : config.tournamentParams.players) {
            Player player(name, &stadium, stadiumTurnstile);
            isRoot = initPlayers(player);
            if (!isRoot) {
                // Player completed the expected matches amount
                exit(0);
            }
        }
    }

    // Interrumption handler
    SIGINT_Handler handler;
    SignalHandler::getInstance()->registrarHandler(SIGINT, (EventHandler *) &handler);

    // FIXME - DO NOT DO THIS!
    for (int i = 0; i < 20; i++) {
        wait(nullptr);
    }

    cout << "FINALIZANDO...";

    return 0;
}