#include "ManagerProcess.h"
#include "../util/Logger.h"
#include "../util/PlayerUtils.h"
#include "../util/ResourceHandler.h"

static const string TAG = "Organizador: ";

ManagerProcess::ManagerProcess(vector<Player> *players, Pipe *pipeFromClient, Stadium *stadium, int maxMatches,
                               int stadiumCapacity) :
        vPlayers(players),
        taskQueue(pipeFromClient),
        stadium(stadium),
        scoreBoardSize(vPlayers->size()),
        maxMatches(maxMatches),
        stadiumCapacity(stadiumCapacity),
        tournamentEnded(false) {
    // Init players
    for (int i = 0; i < vPlayers->size(); i++) {
        for (int j = 0; j < vPlayers->size(); j++) {
            if (i != j) {
                vPlayers->at(i).addPotentialPartner(vPlayers->at(j).getId());
            }
        }
    }
    securityGuard = new SecurityGuard(vPlayers);
    matchMaker = new MatchMaker(vPlayers, stadium);
}

int ManagerProcess::start() {
    int pid = fork();
    if (pid == 0) {
        scoreBoard = ResourceHandler::getVectorCompartido();
        lock = ResourceHandler::getLockFile();
        Logger::d(TAG + "listo para recibir tareas");
        while (!vPlayers->empty()) {
            TaskRequest task = receiveTask();
            switch (task.task) {
                case FIND_PARTNER:
                    onFindPartnerRequest(task.pid);
                    break;
                case MATCH_RESULT:
                    onMatchResultRequest(task.pid, task.resultLocal, task.resultVisitant);
                    break;
                case TIDE_CHANGE:
                    onTideChangeRequest(task.tideRise);
                    break;
            }
        }
        Logger::d(TAG + "todos los jugadores han abandonado el torneo. Listo para finalizar!");
        lock->liberarLock();
        exit(0);
    }
    return pid;
}

TaskRequest ManagerProcess::receiveTask() {
    TaskRequest request{};
    taskQueue->setearModo(Pipe::LECTURA);
    ssize_t out = taskQueue->leer(&request, sizeof(TaskRequest));
    if (out < 0) {
        Logger::e(TAG + " fallo la recepcion de una tarea!!!");
        throw runtime_error(TAG + " fallo la recepcion de una tarea!!!");
    }
    Logger::d(TAG + "tarea recibida correctamente.\n" + request.toString());
    return request;
}

void ManagerProcess::onFindPartnerRequest(int playerId) {
    int idx = findPlayerById(*vPlayers, playerId);
    vPlayers->at(idx).setState(Player::State::WAITING_FOR_PARTNER);

    // Player leaves TOURNAMENT
    //      Case 1: tournament ended. For example, because one existing player doesn't have a potential partner in the tournament
    //      Case 2: played K matches
    if (tournamentEnded || vPlayers->at(idx).getMatchesCount() == maxMatches) {
        securityGuard->dismissPlayerFromTournament(playerId);
        return;
    }
    //      Case 3: player don't have potential partners
    if (vPlayers->at(idx).getPotentialPartners().empty()) {
        matchMaker->releasePlayersWithoutMatch();
        securityGuard->dismissPlayersWaitingFromTournament();
        tournamentEnded = true;
        return;
    }

    if (!matchMaker->findPartnerAvailable(playerId)) {
        // Partner not found
        if (!isPlayerOutside()) {
            // Case 4: player don't have potential partner available and none is waiting OUTSIDE
            matchMaker->releasePlayersWithoutMatch();
            securityGuard->dismissPlayersWaitingFromTournament();
            tournamentEnded = true;
        } else if (isStadiumFull()) {
            // Random player leaves STADIUM
            securityGuard->dismissPlayerFromStadium();
        } else {
            // Player stays waiting for a partner to be assignated to him
            Logger::d(TAG + vPlayers->at(idx).getName() + " esta esperando por un compañero");
        }
        return;
    }

    // Check if there is a match already set up and an empty field
    matchMaker->checkFieldAssignation();
}

void ManagerProcess::onMatchResultRequest(int fieldId, int localScore, int visitantScore) {
    Field *field = stadium->getFieldById(fieldId);

    // Update field state
    field->setState(Field::State::FREE);

    // Points
    int pointsToLocal, pointsToVisitant;
    if (localScore > visitantScore) {
        if (visitantScore == 2) {
            pointsToLocal = 2;
            pointsToVisitant = 1;
        } else {
            pointsToLocal = 3;
            pointsToVisitant = 0;
        }
    } else {
        if (localScore == 2) {
            pointsToLocal = 1;
            pointsToVisitant = 2;
        } else {
            pointsToLocal = 0;
            pointsToVisitant = 3;
        }
    }

    Match match = field->getMatch();
    // Local team update
    //      First player
    int idx = findPlayerById(*vPlayers, match.local.firstPlayerId);
    if (idx == -1) {
        Logger::e(TAG + "Jugador " + to_string(match.local.firstPlayerId) + "no se encuentra dentro del torneo");
        throw runtime_error("Jugador " + to_string(match.local.firstPlayerId) + "no se encuentra dentro del torneo");
    }

    vPlayers->at(idx).increaseMatchesCount();
    vPlayers->at(idx).increasePoints(pointsToLocal);
    vPlayers->at(idx).removePotentialPartner(match.local.secondPlayerId);
    //      Second player
    idx = findPlayerById(*vPlayers, match.local.secondPlayerId);
    if (idx == -1) {
        Logger::e(TAG + "Jugador " + to_string(match.local.secondPlayerId) + "no se encuentra dentro del torneo");
        throw runtime_error("Jugador " + to_string(match.local.secondPlayerId) + "no se encuentra dentro del torneo");
    }
    vPlayers->at(idx).increaseMatchesCount();
    vPlayers->at(idx).increasePoints(pointsToLocal);
    vPlayers->at(idx).removePotentialPartner(match.local.firstPlayerId);
    // Visitant team update
    //      First player
    idx = findPlayerById(*vPlayers, match.visitant.firstPlayerId);
    if (idx == -1) {
        Logger::e(TAG + "Jugador " + to_string(match.visitant.firstPlayerId) + "no se encuentra dentro del torneo");
        throw runtime_error("Jugador " + to_string(match.visitant.firstPlayerId) + "no se encuentra dentro del torneo");
    }
    vPlayers->at(idx).increaseMatchesCount();
    vPlayers->at(idx).increasePoints(pointsToVisitant);
    vPlayers->at(idx).removePotentialPartner(match.visitant.secondPlayerId);
    //      Second player
    idx = findPlayerById(*vPlayers, match.visitant.secondPlayerId);
    if (idx == -1) {
        Logger::e(TAG + "Jugador " + to_string(match.visitant.secondPlayerId) + "no se encuentra dentro del torneo");
        throw runtime_error(
                "Jugador " + to_string(match.visitant.secondPlayerId) + "no se encuentra dentro del torneo");
    }
    vPlayers->at(idx).increaseMatchesCount();
    vPlayers->at(idx).increasePoints(pointsToVisitant);
    vPlayers->at(idx).removePotentialPartner(match.visitant.firstPlayerId);

    // Refresh tournament positions' board
    refreshScoreBoard(match);
}

void ManagerProcess::onTideChangeRequest(bool rise) {
    // TODO - Implement
}

bool ManagerProcess::isStadiumFull() {
    vector<Player> playersOutside = findPlayersByState(*vPlayers, Player::State::OUTSIDE);
    int count = vPlayers->size() - playersOutside.size();
    Logger::d(TAG + "Jugadores dentro del predio: " + to_string(count));
    return count == stadiumCapacity;
}

bool ManagerProcess::isPlayerOutside() {
    vector<Player> playersOutside = findPlayersByState(*vPlayers, Player::State::OUTSIDE);
    int count = playersOutside.size();
    Logger::d(TAG + "Jugadores fuera del predio: " + to_string(count));
    return count > 0;
}

void ManagerProcess::refreshScoreBoard(Match match) {
    int res = lock->tomarLockEscritura();
    if (res == -1) {
        Logger::e(TAG + "Fallo el lock para la Tabla de posiciones");
        throw runtime_error(TAG + "Fallo el lock para la Tabla de posiciones");
    }
    for (int i = 0; i < scoreBoardSize; i++) {
        Player p = scoreBoard->leer(i);
        if (p.getId() == match.local.firstPlayerId) {
            int idx = findPlayerById(*vPlayers, match.local.firstPlayerId);
            if (idx == -1) {
                Logger::e(
                        TAG + "Jugador " + to_string(match.local.firstPlayerId) + "no se encuentra dentro del torneo");
                throw runtime_error(
                        "Jugador " + to_string(match.local.firstPlayerId) + "no se encuentra dentro del torneo");
            }
            Player real = vPlayers->at(idx);
            scoreBoard->escribir(real, i);
        } else if (p.getId() == match.local.secondPlayerId) {
            int idx = findPlayerById(*vPlayers, match.local.secondPlayerId);
            if (idx == -1) {
                Logger::e(
                        TAG + "Jugador " + to_string(match.local.secondPlayerId) + "no se encuentra dentro del torneo");
                throw runtime_error(
                        "Jugador " + to_string(match.local.secondPlayerId) + "no se encuentra dentro del torneo");
            }
            Player real = vPlayers->at(idx);
            scoreBoard->escribir(real, i);
        } else if (p.getId() == match.visitant.firstPlayerId) {
            int idx = findPlayerById(*vPlayers, match.visitant.firstPlayerId);
            if (idx == -1) {
                Logger::e(TAG + "Jugador " + to_string(match.visitant.firstPlayerId) +
                          "no se encuentra dentro del torneo");
                throw runtime_error(
                        "Jugador " + to_string(match.visitant.firstPlayerId) + "no se encuentra dentro del torneo");
            }
            Player real = vPlayers->at(idx);
            scoreBoard->escribir(real, i);
        } else if (p.getId() == match.visitant.secondPlayerId) {
            int idx = findPlayerById(*vPlayers, match.visitant.secondPlayerId);
            if (idx == -1) {
                Logger::e(TAG + "Jugador " + to_string(match.visitant.secondPlayerId) +
                          "no se encuentra dentro del torneo");
                throw runtime_error(
                        "Jugador " + to_string(match.visitant.secondPlayerId) + "no se encuentra dentro del torneo");
            }
            Player real = vPlayers->at(idx);
            scoreBoard->escribir(real, i);
        }
    }
    lock->liberarLock();
}
