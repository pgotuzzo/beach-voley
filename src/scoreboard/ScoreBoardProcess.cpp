#include "ScoreBoardProcess.h"
#include "../util/Logger.h"
#include "../ipc/signal/SignalHandler.h"
#include "Scoreboard_SIGINT_Handler.h"
#include "../util/ResourceHandler.h"

const string TAG = "Tabla de Posiciones: ";

int comparePlayers(Player p1, Player p2) {
    return p1.getPoints() < p2.getPoints();
}

ScoreBoardProcess::ScoreBoardProcess(int playerCount) : playerCount(playerCount), quit(false) {}


int ScoreBoardProcess::start() {
    int pid = fork();
    if (pid == 0) {

        // Init
        vPlayers = ResourceHandler::getVectorCompartido();
        lock = ResourceHandler::getLockFile();

        // Signal handler in order to avoid ipc resources being wasted
        Scoreboard_SIGINT_Handler handler(&quit);
        SignalHandler::getInstance()->registrarHandler(SIGINT, (EventHandler *) &handler);

        while (!quit) {
            show();
            usleep(5000);
        }
        exit(0);
    }
    return pid;
}

void ScoreBoardProcess::show() {
    int res = lock->tomarLockLectura();
    if (res == -1) {
        Logger::e(TAG + "Fallo el lock para la Tabla de posiciones. Error: " + strerror(errno));
        throw runtime_error(TAG + "Fallo el lock para la Tabla de posiciones");
    }
    vector<Player> auxPlayers(playerCount);
    for (int i = 0; i < playerCount; i++) {
        Player p = vPlayers->leer(i);
        auxPlayers[i] = p;
    }

    sort(auxPlayers.begin(), auxPlayers.end(), comparePlayers);

    cout << "=====================================================" << endl;
    cout << " JUGADOR        PARTIDOS JUGADOS     PUNTOS" << endl;
    cout << "-----------------------------------------------------" << endl;
    for (Player p : auxPlayers) {
        string spaces;
        int spacesCount = 20 - p.getName().length();
        for (int i = 0; i < spacesCount; i++) {
            spaces += " ";
        }
        cout << " " << p.getName() << spaces << p.getMatchesCount() << " \t\t\t" << p.getPoints() << endl;
    }
    lock->liberarLock();
}

