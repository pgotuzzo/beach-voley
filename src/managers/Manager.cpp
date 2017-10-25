#include <cstring>

#include <algorithm>
#include "Manager.h"
#include "../logger/Logger.h"
#include "../util/RandomNumber.h"

using namespace std;

/**
 * Logs a message with aditional information about the process and class.
 * @param message the message to log
 */
void logMessage(const string &message) {
    string messageToLog = to_string(getpid()) + string(" Manager: ") + message;
    Logger::getInstance()->logMessage(messageToLog.c_str());
}

/**
 * Manager constructor initialize the attributes.
 *
 * @param totalFields the total number of fields
 * @param capacity the maximum number of players inside the stadium
 * @param matches the maximum number of matches a player can play before leaving the tournament
 * @param players the number of players registered the tournament
 * @param lockForSharedMemory lock to write or read in the shared memory
 * @param pointsTable shared vector with the points for every player
 * @param receiveTaskPipe pipe where the manager receives the tasks
 * @param playersIdPipeMap vector with the pipe where the manager send responses for the players
 */
Manager::Manager(unsigned int totalFields, unsigned int capacity, unsigned int matches, unsigned int players,
                 LockFile *lockForSharedMemory, VectorCompartido<int> *pointsTable, Pipe *receiveTaskPipe,
                 vector<Pipe *> playersIdPipeMap) :
        playersInTournament(players), receiveTaskPipe(receiveTaskPipe), playersGames(vector<int>(players, 0)),
        totalMatchesPerPlayer(matches), fieldsInformation(vector<FieldInformation>(totalFields)),
        stadiumSize(capacity), lockForSharedMemory(lockForSharedMemory), pointsTable(pointsTable) {

    for (unsigned long i = 0; i < playersInTournament; i++) {
//        playersAvailablePartners.push_back(vector<bool>(playersInTournament, true));
        playersAvailablePartners.emplace_back(playersInTournament, true);
        playersAvailablePartners[i][i] = false;
    }

    for (auto playerPipe: playersIdPipeMap) {
        playerPipe->setearModo(Pipe::ESCRITURA);
    }
    receiveTaskPipe->setearModo(Pipe::LECTURA);

    for (unsigned int i = 0; i < playersInTournament; ++i) {
        pointsTable->escribir(0, i);
    }
}

/**
 * Start receiving tasks till the end of the tournament. Reads messages from the task pipe, then tries to solve
 * the task, check if there are still players in the tournament or games in play and reads a new task.
 */
void Manager::receiveTask() {
    ssize_t out = 0;
    while (playersInTournament >= 4 or gamesInPlay > 0 or !playersReturningFromField.empty()) {
        TaskRequest task{};
        logMessage("Trying to read a task");
        out = receiveTaskPipe->leer(static_cast<void *>(&task), sizeof(TaskRequest));
        if (out > 0) {
            logMessage(string("Received a task: ") + task.show());
            switch (task.task) {
                case (FIND_PARTNER):
                    findMatch(task.id);
                    break;
                case (TIDE_CHANGE):
                    updateFieldList(task.id, task.tideRise);
                    break;
                case (MATCH_RESULT):
                    saveResult(task.id, task.resultLocal, task.resultVisitant);
                    break;
                default:
                    throw runtime_error("Task handler not defined.");
            }
            logMessage(string("Task completed"));
        } else {
            cout << strerror(errno) << endl;
        }
        removePlayersThatCantPlay();
    }
    logMessage(string("Tournament finish"));
    for (auto player: waitingPlayers) {
        sendMessageToPlayer(player, OrgPlayerResponse{0, ENUM_LEAVE_TOURNAMENT});
    }
    for (auto team: waitingTeams) {
        sendMessageToPlayer(team.idPlayer1, OrgPlayerResponse{0, ENUM_LEAVE_TOURNAMENT});
        sendMessageToPlayer(team.idPlayer2, OrgPlayerResponse{0, ENUM_LEAVE_TOURNAMENT});
    }
}

/**
 * Tries to find a partner for a given player.
 * If the player already played all his games it tells him to leave the tournament.
 * If the player still have games to play it tries to assign him a partner, if it founds a partner
 * tries to assign an opponent team and send them to a field.
 * If it can't found a partner and the stadium is full, removes one waiting player from the stadium
 * and leave this player waiting, after that tries to form new teams to play.
 *
 * @param playerId the id of the player.
 */
void Manager::findMatch(int playerId) {
    if (!tournamentStart) {
        waitingPlayers.push_back(playerId);
        tournamentStart = waitingPlayers.size() >= 9;
    } else {
        auto playerReturnsFromField = find(playersReturningFromField.begin(), playersReturningFromField.end(),
                                           playerId);
        if (playerReturnsFromField != playersReturningFromField.end()) {
            playersReturningFromField.erase(playerReturnsFromField);
        }
        if (playersGames[playerId] == totalMatchesPerPlayer or countAvailablePartners(playerId) == 0) {
            logMessage(string("Player ") + to_string(playerId) +
                       string("already played all the matches allowed or cant play more."));
            sendMessageToPlayer(playerId, OrgPlayerResponse{0, ENUM_LEAVE_TOURNAMENT});
            removePlayerFromAllAvailablePartners(playerId);
            playersInTournament--;
        } else {
            Team localTeam = Team{playerId, -1};
            if (assignPartner(&localTeam)) {
                logMessage(string("Player ") + to_string(playerId) + string(" now has a partner: Player ") +
                           to_string(localTeam.idPlayer2));
                waitingPlayers.erase(find(waitingPlayers.begin(), waitingPlayers.end(), localTeam.idPlayer2));
                if (!waitingTeams.empty()) {
                    if (assignField(localTeam, waitingTeams.back())) {
                        waitingTeams.pop_back();
                    } else {
                        logMessage("Couldn't find a field");
                        waitingTeams.push_back(localTeam);
                    }
                } else {
                    logMessage("Couldn't find an opponent");
                    waitingTeams.push_back(localTeam);
                }
            } else {
                logMessage(string("Player ") + to_string(playerId) + string(" don't have a partner available"));
                waitingPlayers.push_back(playerId);
                if (stadiumIsFull()) {
                    logMessage("Stadium is full removing a random player");
                    removeRandomWaitingPlayer();
                }
            }
        }
    }
}

/**
 * Counts the number of available partners for that player.
 *
 * @param playerId the player to consult the available partners.
 * @return the number of available partners.
 */
int Manager::countAvailablePartners(int playerId) {
    int countAvailablePartners = 0;
    for (auto availablePartner: playersAvailablePartners[playerId]) {
        if (availablePartner) {
            countAvailablePartners++;
        }
    }
    return countAvailablePartners;
}

/**
 * Sends a message to a particular player.
 *
 * @param playerId the id of the player.
 * @param orgPlayerResponse the message to send.
 */
void Manager::sendMessageToPlayer(int playerId, OrgPlayerResponse orgPlayerResponse) {
    ssize_t out = playersIdPipeMap[playerId]->escribir(static_cast<const void *> (&orgPlayerResponse),
                                                       sizeof(OrgPlayerResponse));
    if (out < 0) {
        throw runtime_error("Partner response fifo can't be write!");
    }
}

/**
 * Removes the player from all the lists of possible partners.
 *
 * @param playerId the player id to be removed.
 */
void Manager::removePlayerFromAllAvailablePartners(int playerId) {
    for (auto availablePartners: playersAvailablePartners) {
        availablePartners[playerId] = false;
    }
}

/**
 * Tries to find a partner for a player.
 * Iterates over all the waiting players and checks if any of them can play with the searching team player.
 *
 * @param teamProject a team with the first player as the player searching for team. If the partner is found
 *                      it will be added as player2 in this struct.
 * @return true if a partner was found.
 */
bool Manager::assignPartner(Team *teamProject) {
    vector<bool> &playerAvailablePartners = playersAvailablePartners[teamProject->idPlayer1];
    for (auto waitingPlayer: waitingPlayers) {
        if (playerAvailablePartners[waitingPlayer]) {
            teamProject->idPlayer2 = waitingPlayer;
            return true;
        }
    }
    return false;
}

/**
 * If there is a free field, it's assigned to the teams and sends the players to play the game.
 *
 * @param localTeam the local team for the game
 * @param visitTeam the visit team for the game
 */
bool Manager::assignField(Team localTeam, Team visitTeam) {
    unsigned short i = 0;
    bool assignedField = false;
    while (i < fieldsInformation.size() and !assignedField) {
        if (fieldsInformation[i].isFree) {
            logMessage(string("Sending players to field ") + to_string(i) + string(" Local team: players ") +
                       to_string(localTeam.idPlayer1) + string(" and ") + to_string(localTeam.idPlayer2) +
                       string(" Visit team: players ") + to_string(visitTeam.idPlayer1) + string(" and ") +
                       to_string(visitTeam.idPlayer2));
            fieldsInformation[i].isFree = false;
            fieldsInformation[i].localTeam = localTeam;
            fieldsInformation[i].visitTeam = visitTeam;
            sendPlayersToField(i);
            gamesInPlay++;
            assignedField = true;
        }
        i++;
    }
    return assignedField;
}

/**
 * Sends message to every player to play in the given field.
 *
 * @param fieldId the id of the field to send the players, the field information must be updated
 */
void Manager::sendPlayersToField(unsigned short fieldId) {
    OrgPlayerResponse orgPlayerResponse = OrgPlayerResponse{fieldId, ENUM_PLAY};
    sendMessageToPlayer(fieldsInformation[fieldId].localTeam.idPlayer1, orgPlayerResponse);
    sendMessageToPlayer(fieldsInformation[fieldId].localTeam.idPlayer2, orgPlayerResponse);
    sendMessageToPlayer(fieldsInformation[fieldId].visitTeam.idPlayer1, orgPlayerResponse);
    sendMessageToPlayer(fieldsInformation[fieldId].visitTeam.idPlayer2, orgPlayerResponse);
}

/**
 * Checks if the stadium is full.
 *
 * @return returns true if the stadium is full.
 */
bool Manager::stadiumIsFull() {
    return waitingPlayers.size() + playersReturningFromField.size() + 2 * waitingTeams.size() + 4 * gamesInPlay ==
           stadiumSize;
}

/**
 * Picks a random player from the waiting players and send him out of the stadium.
 */
void Manager::removeRandomWaitingPlayer() {
    int randomPlayer = getRandomInt(0, static_cast<int>(waitingPlayers.size()));
    sendMessageToPlayer(waitingPlayers[randomPlayer], OrgPlayerResponse{0, ENUM_LEAVE_STADIUM});
    waitingPlayers.erase(find(waitingPlayers.begin(), waitingPlayers.end(), waitingPlayers[randomPlayer]));
}

/**
 * Updates the field list for a tide change.
 *
 * @param fieldId the id of the field to update.
 * @param tideRise true if the tide rise or false if the tide fall.
 */
void Manager::updateFieldList(int fieldId, bool tideRise) {
    // If the tide rises in a field that was in a game, the game was interrupted.
    if (tideRise and !fieldsInformation[fieldId].isFree) {
        gamesInPlay--;
    }
    fieldsInformation[fieldId].isFree = !tideRise;
}

/**
 * Saves the results of the match, and stores the players that played together.
 * Frees the field. Saves the results of the match in a history and the points
 * of the players in the shared memory.
 *
 * @param fieldId the id of the field.
 * @param resultLocal the number of sets won by the local team.
 * @param resultVisitant the number of sets won by the visit team.
 */
void Manager::saveResult(int fieldId, int resultLocal, int resultVisitant) {
    fieldsInformation[fieldId].isFree = true;
    Team localTeam = fieldsInformation[fieldId].localTeam;
    Team visitTeam = fieldsInformation[fieldId].visitTeam;
    removePlayersFromAvailablePartners(localTeam);
    removePlayersFromAvailablePartners(visitTeam);
    gamesInPlay--;
    playersGames[localTeam.idPlayer1]++;
    playersGames[localTeam.idPlayer2]++;
    playersGames[visitTeam.idPlayer1]++;
    playersGames[visitTeam.idPlayer2]++;
    playersReturningFromField.push_back(localTeam.idPlayer1);
    playersReturningFromField.push_back(localTeam.idPlayer2);
    playersReturningFromField.push_back(visitTeam.idPlayer1);
    playersReturningFromField.push_back(visitTeam.idPlayer2);

    logMessage("Match between " + to_string(localTeam.idPlayer1) + " and " + to_string(localTeam.idPlayer2) +
               " vs " + to_string(visitTeam.idPlayer1) + " and " + to_string(visitTeam.idPlayer2) +
               " result: " + to_string(resultLocal) + " " + to_string(resultVisitant));

    auto idxLocalPlayer1 = static_cast<unsigned int>(localTeam.idPlayer1);
    auto idxLocalPlayer2 = static_cast<unsigned int>(localTeam.idPlayer2);
    auto idxVisitPlayer1 = static_cast<unsigned int>(visitTeam.idPlayer1);
    auto idxVisitPlayer2 = static_cast<unsigned int>(visitTeam.idPlayer2);

    lockForSharedMemory->tomarLock();
    if (resultLocal == 3) {
        if (resultVisitant == 2) {
            pointsTable->escribir(pointsTable->leer(idxLocalPlayer1) + 2, idxLocalPlayer1);
            pointsTable->escribir(pointsTable->leer(idxLocalPlayer2) + 2, idxLocalPlayer2);
            pointsTable->escribir(pointsTable->leer(idxVisitPlayer1) + 1, idxVisitPlayer1);
            pointsTable->escribir(pointsTable->leer(idxVisitPlayer2) + 1, idxVisitPlayer2);
        } else {
            pointsTable->escribir(pointsTable->leer(idxLocalPlayer1) + 3, idxLocalPlayer1);
            pointsTable->escribir(pointsTable->leer(idxLocalPlayer2) + 3, idxLocalPlayer2);
        }
    } else if (resultLocal == 2) {
        pointsTable->escribir(pointsTable->leer(idxLocalPlayer1) + 1, idxLocalPlayer1);
        pointsTable->escribir(pointsTable->leer(idxLocalPlayer2) + 1, idxLocalPlayer2);
        pointsTable->escribir(pointsTable->leer(idxVisitPlayer1) + 2, idxVisitPlayer1);
        pointsTable->escribir(pointsTable->leer(idxVisitPlayer2) + 2, idxVisitPlayer2);
    } else {
        pointsTable->escribir(pointsTable->leer(idxVisitPlayer1) + 3, idxVisitPlayer1);
        pointsTable->escribir(pointsTable->leer(idxVisitPlayer2) + 3, idxVisitPlayer2);
    }
    lockForSharedMemory->liberarLock();
}

/**
 * Removes the available partners from both players in team.
 *
 * @param team the team to remove their available partners.
 */
void Manager::removePlayersFromAvailablePartners(Team team) {
    playersAvailablePartners[team.idPlayer2][team.idPlayer1] = false;
    playersAvailablePartners[team.idPlayer1][team.idPlayer2] = false;
}

/**
 * Removes from the waiting players all players that cant play more games in the tournament.
 */
void Manager::removePlayersThatCantPlay() {
    vector<int> removePlayers;
    for (auto player: waitingPlayers) {
        if (playersGames[player] == totalMatchesPerPlayer or countAvailablePartners(player) == 0) {
            removePlayers.push_back(player);
        }
    }
    for (auto player: removePlayers) {
        logMessage(string("Player ") + to_string(player) +
                   string("already played all the matches allowed or cant play more."));
        sendMessageToPlayer(player, OrgPlayerResponse{0, ENUM_LEAVE_TOURNAMENT});
        removePlayerFromAllAvailablePartners(player);
        waitingPlayers.erase(find(waitingPlayers.begin(), waitingPlayers.end(), player));
        playersInTournament--;
    }
}
