#include <iostream>
#include <sys/wait.h>
#include "config/Config.h"
#include "../util/StringUtils.h"
#include "court/Field.h"
#include "player/Player.h"
#include "../IPCClasses/FifoWrite.h"
#include "InitException.h"
#include "../IPCClasses/FifoRead.h"
#include "Constants.h"
#include "manager/Manager.h"

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

bool initField(Field field) {
    for (int i = 0; i < field.getColumns(); i++) {
        for (int j = 0; j < field.getRows(); j++) {
            Court court = field.getCourt(i, j);
            int pid = fork();
            if (pid == 0) {
                court.waitForPlayers();
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

        // Field = [C X R] Courts
        Field field(config.tournamentParams.columns, config.tournamentParams.rows);
        bool isRoot = initField(field);
        if (!isRoot) {
            // Tournament ended
            exit(0);
        }

        Manager manager;

        isRoot = initManager(manager);
        if (!isRoot) {
            // Tournament ended
            exit(0);
        }

        // Players
        for (const auto &name : config.tournamentParams.players) {
            Player player(name, &field);
            isRoot = initPlayers(player);
            if (!isRoot) {
                // Player completed the expected matches amount
                exit(0);
            }
        }



    }


    // FIXME - DO NOT DO THIS!
    int k;
    for (int i = 0; i < 20; i++) {
        wait(&k);
    }

    cout << "FINALIZANDO...";
    return 0;
}