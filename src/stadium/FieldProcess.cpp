#include "FieldProcess.h"
#include "../util/Logger.h"
#include "../util/RandomNumber.h"
#include "../cons/Definition.h"

const int PLAYER_PER_MATCH = 4;
const int SETS_TO_WIN = 3;

FieldProcess::FieldProcess(string name, Pipe *managerQueue, int semId, Semaforo *entrance, Semaforo *exit) :
        TAG("Cancha " + name + ": "), managerQueue(managerQueue), semId(semId), entrance(entrance), exit(exit),
        playersInField(0) {}

int FieldProcess::start() {
    int pid = fork();
    if (pid == 0) {
        while (true) {
            waitForPlayers();
            MatchResult matchResult = playMatch();
            sendResult(matchResult);
            releasePlayers();
        }
    }
    return pid;
}

void FieldProcess::waitForPlayers() {
    for (int i = 0; i < PLAYER_PER_MATCH; i++) {
        entrance->p(semId);
        playersInField++;
        Logger::d(TAG + "Recibio un nuevo jugador. Total: " + to_string(playersInField));
    }
}

FieldProcess::MatchResult FieldProcess::playMatch() {
    Logger::d(TAG + "Comienza el partido!");

    usleep(getRandomUnsignedInt(1000, 5000));

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

    Logger::d(TAG + "Finalizo el partido!\n" + result.toString());
    return result;
}

void FieldProcess::releasePlayers() {
    Logger::d(TAG + "Se despiden a los jugadores " + to_string(playersInField));
    for (int i = 0; i < playersInField; i++) {
        exit->v(semId);
    }
    playersInField = 0;
}

void FieldProcess::sendResult(MatchResult matchResult) {
    TaskRequest request{};
    request.task = MATCH_RESULT;
    request.pid = getpid();
    request.resultLocal = matchResult.localScore;
    request.resultVisitant = matchResult.visitantScore;

    managerQueue->setearModo(Pipe::ESCRITURA);
    ssize_t out = managerQueue->escribir(&request, sizeof(TaskRequest));
    if (out < 0) {
        Logger::e(TAG + " fallo el envio de una tarea!!!");
        throw runtime_error(TAG + " fallo el envio de una tarea!!!");
    }
    Logger::d(TAG + "tarea enviada correctamente.\n" + request.toString());
}
