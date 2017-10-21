#include "FieldProcess.h"
#include "../util/Logger.h"
#include "../util/RandomNumber.h"

const int PLAYER_PER_MATCH = 4;
const int SETS_TO_WIN = 3;

FieldProcess::FieldProcess(string name, Pipe *managerQueue, Semaforo *entrance, Semaforo *exit) :
        TAG("Cancha " + name + ": "), managerQueue(managerQueue), entrance(entrance), exit(exit), playersInField(0) {}

int FieldProcess::start() {
    int pid = fork();
    if (pid == 0) {
        while (true) {
            waitForPlayers();
            MatchResult matchResult = playMatch();
            releasePlayers();
            sendResult(matchResult);
        }
    }
    return pid;
}

void FieldProcess::waitForPlayers() {
    for (int i = 0; i < PLAYER_PER_MATCH; i++) {
        entrance->p(0);

        Logger::d(TAG + "Recibio un nuevo jugador. Total: " + to_string(playersInField));

        playersInField++;
    }
}

FieldProcess::MatchResult FieldProcess::playMatch() {
    Logger::d(TAG + "Comienza el partido!");

    // TODO - Add sleep

    bool localWin = getRandomBool();
    int looserScore = getRandomInt(0, SETS_TO_WIN - 1);

    MatchResult result{};
    if (localWin) {
        result.localScore = SETS_TO_WIN;
        result.visitantScore = looserScore;
    } else {
        result.localScore = looserScore;
        result.visitantScore = SETS_TO_WIN;
    }

    Logger::d(TAG + "Finalizo el partido!");
    return result;
}

void FieldProcess::releasePlayers() {
    Logger::d(TAG + "Se despiden a los jugadores" + to_string(playersInField));
    for (int i = 0; i < playersInField; i++) {
        exit->v(0);
    }
}

void FieldProcess::sendResult(MatchResult matchResult) {
    // TODO - Implement
}
