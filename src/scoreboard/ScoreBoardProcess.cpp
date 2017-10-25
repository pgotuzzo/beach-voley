#include "ScoreBoardProcess.h"
#include "../util/Logger.h"
#include "../ipc/signal/SignalHandler.h"
#include "../util/ResourceHandler.h"
#include "../ipc/signal/SIGTERM_Handler.h"

const string TAG = "Tabla de Posiciones: ";

int comparePlayers(PlayerStats p1, PlayerStats p2) {
    return p1.points > p2.points;
}

ScoreBoardProcess::ScoreBoardProcess() : quit(false) {}


int ScoreBoardProcess::start() {
    int pid = fork();
    if (pid == 0) {
        // Init
        vPlayers = ResourceHandler::getVectorCompartido();
        lock = ResourceHandler::getLockFile();

        // Signal handler in order to avoid ipc resources being wasted
        SIGTERM_Handler termHandler;
        SignalHandler::getInstance()->registrarHandler(SIGTERM, (EventHandler *) &termHandler);

        while (termHandler.getGracefulQuit() == 0) {
            show();
            usleep(5000);
        }
        Logger::d(TAG + "Terminando!!");
        exit(0);
    }
    return pid;
}

void ScoreBoardProcess::show() {
    int res = lock->tomarLockEscritura();
    if (res == -1) {
        Logger::e(TAG + "Fallo el lock para la Tabla de posiciones. Error: " + strerror(errno));
        throw runtime_error(TAG + "Fallo el lock para la Tabla de posiciones");
    }

    vector<PlayerStats> auxPlayers;
    for (int i = 0; i < vPlayers->size(); i++) {
        PlayerStats stats = vPlayers->leer(i);
        auxPlayers.push_back(stats);
    }
    res = lock->liberarLock();

    sort(auxPlayers.begin(), auxPlayers.end(), comparePlayers);

    cout << "=====================================================" << endl;
    cout << " JUGADOR        PARTIDOS JUGADOS            PUNTOS" << endl;
    cout << "-----------------------------------------------------" << endl;
    for (int i = 0; i < auxPlayers.size(); i++) {
        PlayerStats stats = auxPlayers[i];
        string spaces;
        int spacesCount = 20 - stats.name.length();
        for (int j = 0; j < spacesCount; j++) {
            spaces += " ";
        }
        cout << i + 1 << ". " << stats.name << spaces << stats.matches << " \t\t\t" << stats.points << endl;
    }
}

