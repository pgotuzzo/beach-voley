#include <iostream>
#include <zconf.h>
#include "config/Config.h"
#include "../util/StringUtils.h"
#include "court/Field.h"
#include "player/Player.h"
#include "../IPCClasses/FifoWrite.h"
#include "InitException.h"
#include "../IPCClasses/FifoRead.h"
#include "Constants.h"

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

bool initPlayers(Player player) {
    int pid = fork();
    if (pid == 0) {
        player.play();
        return false;
    }
    return true;
}

void mock() {
    cout << "MOCK!" << std::endl;


    FifoRead *partnerFifoR = new FifoRead(FIFO_FILE_PARTNER_REQUEST);
    int fd = partnerFifoR->openFifo();
    if (fd < 0) {
        throw InitException("Partner response fifo can't be opened!");
    }

    auto *pid = new int;

    ssize_t out1 = partnerFifoR->readFifo((pid), sizeof(pid));

    if (out1 < 0) {
        throw InitException("Partner request fifo can't be write!");
    }

    partnerFifoR->closeFifo();

    cout << FIFO_FILE_PARTNER_RESPONSE + to_string(*pid) << endl;
    FifoWrite *partnerFifo = new FifoWrite(FIFO_FILE_PARTNER_RESPONSE + to_string(*pid));
    fd = partnerFifo->openFifo();
    int ss = sizeof(to_string(getpid()));

    if (fd < 0) {
        throw InitException("Partner request fifo can't be opened!");
    }

    auto *response = new OrgPlayerResponse;

    response->column = 0;
    response->row = 0;
    response->playerAction = ENUM_PLAY;

    ssize_t out = partnerFifo->writeFifo(static_cast<const void *> (response), sizeof(OrgPlayerResponse));

    if (out < 0) {
        throw InitException("Partner request fifo can't be write!");
    }

    cout << "Participante: pidió un compañero" << endl;

    partnerFifo->closeFifo();
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

        // Players
        for (const auto &name : config.tournamentParams.players) {
            Player player(name, &field);
            isRoot = initPlayers(player);
            if (!isRoot) {
                // Player completed the expected matches amount
                exit(0);
            }
        }

        // FIXME - REMOVE MOCK
        sleep(5);
        mock();

    }


    // FIXME - DO NOT DO THIS!
    int k;
    for (int i = 0; i < 20; i++) {
        wait(&k);
    }

    cout << "FINALIZANDO...";
    return 0;
}