#include <zconf.h>
#include "LauncherProcess.h"
#include "util/Logger.h"
#include "cons/Constants.h"

static const string TAG = "Recepcion: ";

LauncherProcess::LauncherProcess(unsigned short int playersAmount, Semaforo *tournamentSubscription, Semaforo *tournamentStart) :
        playersAmount(playersAmount),
        tournamentSubscription(tournamentSubscription),
        tournamentStart(tournamentStart) {}

int LauncherProcess::start() {
    int pid = fork();
    if (pid == 0) {
        openSubscriptions();
        startTournament();
        exit(0);
    }
    return pid;
}

void LauncherProcess::openSubscriptions() {
    for (int i = 1; i <= TOURNAMENT_MIN_PLAYERS_TO_START; i++) {
        tournamentSubscription->p(0);
        Logger::d(TAG + "Nuevo participante inscripto. Total inscriptos: " + to_string(i));
    }
}

void LauncherProcess::startTournament() {
    Logger::d(TAG + "Comienza el torneo!");
    for (int i = 0; i < playersAmount; i ++) {
        tournamentStart->v(0);
    }
}
