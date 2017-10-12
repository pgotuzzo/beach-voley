#include <cstring>

#include <algorithm>
#include "Manager.h"
#include "../../util/ResourceHandler.h"
#include "../../util/RandomNumber.h"

const char *TAG = "Manager: ";

Manager::Manager(TournamentParams tournamentParams, VectorCompartido<int> *idsTable, VectorCompartido<int> *pointsTable,
                 LockFile *lockForSharedVectors, Pipe *receiveTaskPipe, map<int, Pipe *> playersIdPipeMap) :
        stadiumSize(tournamentParams.capacity), totalGames(tournamentParams.matches),
        totalPlayersInTournament(tournamentParams.players.size()), rows(tournamentParams.rows),
        columns(tournamentParams.columns), idsTable(idsTable), pointsTable(pointsTable),
        receiveTaskPipe(receiveTaskPipe), lockForSharedVectors(lockForSharedVectors),
        playersIdPipeMap(playersIdPipeMap), initialPlayersInTournament(tournamentParams.players.size()) {
    this->freeFields = vector<bool>(rows * columns, true);
    this->teamsOnFields = vector<TeamsMatch>(rows * columns);

    vector<int> valuesPlayers;
    for (int i = 0; i < totalPlayersInTournament; i++) {
        valuesPlayers.push_back(i);
    }
    for (int i = 0; i < totalPlayersInTournament; i++) {
        this->playersPossiblePartners.emplace(i, valuesPlayers);
    }
}

/**
 * Creates a separated process and starts to receive tasks.
 */
void Manager::initManager() {
    int pid = fork();
    if (pid == 0) {
        this->receiveTask();
        ResourceHandler::getInstance()->freeResources();
        exit(0);
    }
}

/**
 * Start receiving tasks till the end of the tournament.
 * Reads messages from the task fifo, then tries to solve the task,
 * check if the tournament finishes and reads a new task.
 */
void Manager::receiveTask() {
    while (checkTournamentEnd()) {
        count++;
        TaskRequest task{};
        cout << TAG << "Trying to read a task" << endl;
        ssize_t out = receiveTaskPipe->leer(static_cast<void *>(&task), sizeof(TaskRequest));
        cout << TAG << "Read something... :thinking: " << task.show() << " out: " << out << endl;

        if (out > 0) {
            cout << TAG << "Read a task successfully!" << endl;
            switch (task.task) {
                case (FIND_PARTNER):
                    cout << TAG << "Task: FIND PARTNER" << endl;
                    findPartner(task.id);
                    break;
                case (TIDE_CHANGE):
                    cout << TAG << "Task: TIDE CHANGE" << endl;
                    updateFieldList(task.id, task.tideRise);
                    break;
                case (MATCH_RESULT):
                    cout << TAG << "Task: MATCH RESULT" << endl;
                    saveResult(task.id, task.resultLocal, task.resultVisitant);
                    break;
                default:
                    throw runtime_error("Task handler not defined.");
            }
            cout << TAG << "Task completed! Going for a new one" << endl;
        } else {
            cout << strerror(errno)<< endl;
        }
    }
    cout << "Tournament ended!" << endl;
}

/**
 * Updates the field list for a tide change.
 *
 * @param fieldId the id of the field to update.
 * @param tideRise true if the tide rise or false if the tide fall.
 */
void Manager::updateFieldList(int fieldId, bool tideRise) {
    freeFields[fieldId] = !tideRise;
}

/**
 * Removes a player from the possible partner of other player.
 *
 * @param targetPlayer the player from whom the possible partner will be removed.
 * @param playerToRemove the player to remove from the other player partner.
 */
void Manager::removePlayerFromPossiblePartner(int targetPlayer, int playerToRemove) {
    playersPossiblePartners[targetPlayer].erase(find(playersPossiblePartners[targetPlayer].begin(),
                                                     playersPossiblePartners[targetPlayer].end(),
                                                     playerToRemove));
}


/**
 * Removes the players of a team from their respective possible partner lists.
 *
 * @param team the team to erase their partners.
 */
void Manager::removePlayersFromPossiblePartners(Team team) {
    cout << "Team: player " << team.idPlayer1 << " - player " << team.idPlayer2 << endl
         << "Player " << team.idPlayer1 << " available partners: " << endl;
    for (int p : playersPossiblePartners[team.idPlayer1]) {
        cout << " Player " << p << endl;
    }

    cout << "Player " << team.idPlayer2 << " available partners: " << endl;
    for (int p : playersPossiblePartners[team.idPlayer2]) {
        cout << " Player " << p << endl;
    }
    removePlayerFromPossiblePartner(team.idPlayer1, team.idPlayer2);
    removePlayerFromPossiblePartner(team.idPlayer2, team.idPlayer1);
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
    TeamsMatch teamsMatch = teamsOnFields[fieldId];
    freeFields[fieldId] = true;
    removePlayersFromPossiblePartners(teamsMatch.localTeam);
    removePlayersFromPossiblePartners(teamsMatch.visitTeam);
    playersInGame = playersInGame - 4;

    auto idxLocalPlayer1 = static_cast<unsigned int>(idToVectorIndexMap[teamsMatch.localTeam.idPlayer1]);
    auto idxLocalPlayer2 = static_cast<unsigned int>(idToVectorIndexMap[teamsMatch.localTeam.idPlayer2]);
    auto idxVisitPlayer1 = static_cast<unsigned int>(idToVectorIndexMap[teamsMatch.visitTeam.idPlayer1]);
    auto idxVisitPlayer2 = static_cast<unsigned int>(idToVectorIndexMap[teamsMatch.visitTeam.idPlayer2]);

    lockForSharedVectors->tomarLock();
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
    lockForSharedVectors->liberarLock();
    matchHistory.push_back(MatchResult{teamsMatch, resultLocal, resultVisitant});
}

/**
 * Sends a message to a particular player.
 *
 * @param playerId the id of the player.
 * @param orgPlayerResponse the message to send.
 */
void Manager::sendMessageToPlayer(int playerId, OrgPlayerResponse orgPlayerResponse) {
    cout << TAG << "Trying to write a response" << endl;
    ssize_t out = playersIdPipeMap[playerId]->escribir(static_cast<const void *> (&orgPlayerResponse),
                                                       sizeof(OrgPlayerResponse));
    if (out < 0) {
        throw runtime_error("Partner response fifo can't be write!");
    }
    cout << TAG << "Response written successfully" << endl;
}

/**
 * Sends message to every player to play in the given field.
 *
 * @param teamsMatch the teams to play the game.
 * @param column the column of the assigned field.
 * @param row the row of the assigned field.
 */
void Manager::sendPlayersToField(TeamsMatch teamsMatch, int fieldId) {
    OrgPlayerResponse orgPlayerResponse = OrgPlayerResponse{fieldId, ENUM_PLAY};
    sendMessageToPlayer(teamsMatch.localTeam.idPlayer1, orgPlayerResponse);
    sendMessageToPlayer(teamsMatch.localTeam.idPlayer2, orgPlayerResponse);
    sendMessageToPlayer(teamsMatch.visitTeam.idPlayer1, orgPlayerResponse);
    sendMessageToPlayer(teamsMatch.visitTeam.idPlayer2, orgPlayerResponse);
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
    auto waitingPlayer = waitingPlayers.begin();
    while (waitingPlayer != waitingPlayers.end()) {
        auto partnerId = find(playersPossiblePartners[teamProject->idPlayer1].begin(),
                              playersPossiblePartners[teamProject->idPlayer1].end(), *waitingPlayer);
        // If its the player do the search again from the player on.
        if (*partnerId == teamProject->idPlayer1) {
            partnerId = find(partnerId, playersPossiblePartners[teamProject->idPlayer1].end(), *waitingPlayer);
        }
        if (partnerId != playersPossiblePartners[teamProject->idPlayer1].end()) {
            teamProject->idPlayer2 = *partnerId;
            return true;
        }
        waitingPlayer++;
    }
    return false;
}

/**
 * If there is any waiting team returns this team as opponent.
 *
 * @param teamProject an team struct to fill the opponents.
 * @return true if it found any team.
 */
bool Manager::findOpponents(Team *teamProject) {
    if (waitingTeams.empty()) {
        return false;
    }
    teamProject->idPlayer1 = waitingTeams[0].idPlayer1;
    teamProject->idPlayer2 = waitingTeams[0].idPlayer2;
    return true;
}

/**
 * If there is a free field, it's assigned to the teams and sends the players to play the game.
 *
 * @param localTeam the local team for the game
 * @param visitTeam the visit team for the game
 */
bool Manager::assignField(Team localTeam, Team visitTeam) {
    int i = 0;
    bool assignedField = false;
    while (i < rows * columns and !assignedField) {
        if (freeFields[i]) {
            freeFields[i] = false;
            TeamsMatch teamsMatch = TeamsMatch{localTeam, visitTeam};
            stringstream m;
            m << "===========================================================================" << endl
              << "Enaviando Equipos a la cancha: " << i << endl
              << "Local: Jugador " << localTeam.idPlayer1 << " - Jugador " << localTeam.idPlayer2 << endl
              << "Visitante: Jugador " << visitTeam.idPlayer1 << " - Jugador " << visitTeam.idPlayer2 << endl
              << "===========================================================================" << endl;
            Logger::getInstance()->logMessage(m.str().c_str());
            teamsOnFields[i] = teamsMatch;
            sendPlayersToField(teamsMatch, i);
            playersInGame = playersInGame + 4;
            assignedField = true;
        }
        i++;
    }
    return assignedField;
}

/**
 * Checks if the stadium is full.
 *
 * @return returns true if the stadium is full.
 */
bool Manager::stadiumIsFull() {
    return waitingPlayers.size() + 2 * waitingTeams.size() + playersInGame + 1 == stadiumSize;
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
 * It will try to form teams with all the waiting players. It will try to assign fields to all the waiting teams.
 */
void Manager::formTeamsAndAssignFields() {
    bool teamFormed = true;
    // Iterates this way, because removing elements in different places while iterating can cause crashes.
    // So they iterate till they cant form a new team.
    while (teamFormed) {
        auto waitingPlayer = waitingPlayers.begin();
        Team team{*waitingPlayer, 0};
        teamFormed = false;
        while (waitingPlayer != waitingPlayers.end() and !teamFormed) {
            teamFormed = assignPartner(&team);
            waitingPlayer++;
        }
        if (teamFormed) {
            waitingPlayers.erase(find(waitingPlayers.begin(), waitingPlayers.end(), team.idPlayer1));
            waitingPlayers.erase(find(waitingPlayers.begin(), waitingPlayers.end(), team.idPlayer2));
            waitingTeams.push_back(team);
        }
    }
    bool assignedField = true;
    while (waitingTeams.size() >= 2 and assignedField) {
        assignedField = assignField(waitingTeams[waitingTeams.size() - 1], waitingTeams[waitingTeams.size() - 2]);
        if (assignedField) {
            waitingTeams.pop_back();
            waitingTeams.pop_back();
        }
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
void Manager::findPartner(int playerId) {
    if (playerPlayAllGamesOrHasNoPossiblePartner(playerId)) {
        // LEAVE TOURNAMENT - All matches played
        // LEAVE TOURNAMENT - Or no more players to play
        cout << TAG << "Player " << playerId
             << "already played all the matches allowed or cant play more. Dismissing him from tournament!" << endl;
        sendMessageToPlayer(playerId, OrgPlayerResponse{0, ENUM_LEAVE_TOURNAMENT});
        removePlayerFromAllPossiblePartners(playerId);
        totalPlayersInTournament--;
    } else {
        // FIND PARTNER - Matches to play
        Team localTeam = Team{playerId, 0};
        if (assignPartner(&localTeam)) {
            // PARTNER FOUND - Local team ready
            cout << TAG << "Player " << playerId << " now has a partner: Player " << localTeam.idPlayer2 << endl;
            waitingPlayers.erase(find(waitingPlayers.begin(), waitingPlayers.end(), localTeam.idPlayer2));
            Team opponentTeam{};
            if (findOpponents(&opponentTeam)) {
                if (assignField(localTeam, opponentTeam)) {
                    waitingTeams.erase(find(waitingTeams.begin(), waitingTeams.end(), opponentTeam));
                } else {
                    waitingPlayers.erase(find(waitingPlayers.begin(), waitingPlayers.end(), localTeam.idPlayer2));
                    waitingTeams.push_back(localTeam);
                }
            } else {
                cout << TAG << "Couldn't find an opponent" << endl;
                waitingTeams.push_back(localTeam);
            }
        } else {
            cout << TAG << "Player " << playerId << " don't have a partner available. :sad: " << endl;
            // PARTNER NOT FOUND - Team ready
            if (stadiumIsFull()) {
                cout << TAG << "Stadium is full...removing a (random) player!" << endl;
                if (removePlayersThatCantPlay()) {
                    removeRandomWaitingPlayer();
                }
            }
            waitingPlayers.push_back(playerId);
        }
    }
}

/**
 * Checks if the given player have played all his games.
 *
 * @param playerId the player to check.
 * @return true if the player played all his games.
 */
bool Manager::playerPlayAllGamesOrHasNoPossiblePartner(int playerId) {
    bool playedAllGames = initialPlayersInTournament - 1 - playersPossiblePartners[playerId].size() >= totalGames;
    if (playedAllGames) {
        cout << "totalPlayersInTournament" << to_string(initialPlayersInTournament) << endl;
        cout << "totalGames" << to_string(totalGames) << endl;
        cout << "playersPossiblePartners[playerId].size()" << to_string(playersPossiblePartners[playerId].size()) << endl;
    }
    // has no possible partner if the only possible partner is himself.
    bool hasNoPossiblePartner = playersPossiblePartners[playerId].size() == 1;
    if (hasNoPossiblePartner) {

        cout << "hasNoPossiblePartner" << endl;
    }
    return playedAllGames or hasNoPossiblePartner;
}

/**
 * Checks if there are no more players in the tournament so the tournament ends.
 *
 * @return true if the tournament doesnt ends yet.
 */
bool Manager::checkTournamentEnd() {
    return totalPlayersInTournament > 0;
}

/**
 * Removes the player for all the lists of possible partners.
 *
 * @param playerId the player id to be removed.
 */
void Manager::removePlayerFromAllPossiblePartners(int playerId) {
    for (const auto &player: playersPossiblePartners) {
        removePlayerFromPossiblePartner(player.first, playerId);
    }
}

/**
 * Removes from the waiting players all players that cant play more games in the tournament.
 *
 * @return true if it removes some player.
 */
bool Manager::removePlayersThatCantPlay() {
    bool playerRemoved = false;
    vector<int> removePlayers;
    for (auto player: waitingPlayers) {
        if (playerPlayAllGamesOrHasNoPossiblePartner(player)) {
            removePlayers.push_back(player);
            playerRemoved = true;
        }
    }
    for (auto player: removePlayers) {
        cout << TAG << "Player " << player
             << "already played all the matches allowed or cant play more. Dismissing him from tournament!" << endl;
        sendMessageToPlayer(player, OrgPlayerResponse{0, ENUM_LEAVE_TOURNAMENT});
        removePlayerFromAllPossiblePartners(player);
        totalPlayersInTournament--;
    }
    return playerRemoved;
}
