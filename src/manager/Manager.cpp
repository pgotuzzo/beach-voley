#include <cstring>

#include <algorithm>
#include "Manager.h"
#include "../config/Constants.h"
#include "../../util/ResourceHandler.h"
#include "../../util/RandomNumber.h"

const char *TAG = "Manager: ";

Manager::Manager(unsigned int rows, unsigned int columns, unsigned int stadiumSize, unsigned int totalGames) :
        stadiumSize(stadiumSize), totalGames(totalGames) {
    this->receiveTaskFifo = ResourceHandler::getInstance()->createFifoRead(FIFO_FILE_MANAGER_RECEIVE_TASK);
    this->rows = rows;
    this->columns = columns;
    this->freeFields = vector<bool>(rows * columns, true);
    this->teamsOnFields = vector<TeamsMatch>(rows * columns);
}

/**
 * Creates a separated process and starts to receive tasks.
 */
void Manager::initManager() {
    int pid = fork();
    if (pid == 0) {
        this->receiveTask();
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
        cout << TAG << "Trying to open a fifo to read a task N°: " << count << endl;
        int fd = receiveTaskFifo->openFifo();
        if (fd < 0) {
            stringstream message;
            message << TAG << "Trying to open a fifo to read a task. Fifo couldn't be opened. Error Number: "
                    << strerror(errno) << " tamaño: " << count << endl;
            throw runtime_error(message.str());
        }

        TaskRequest task = {};
        cout << TAG << "Trying to read a task" << endl;
        ssize_t out = receiveTaskFifo->readFifo((&task), sizeof(TaskRequest));
        cout << TAG << "Read something... :thinking: " << task.show() << " out:" << out << endl;
        receiveTaskFifo->closeFifo();

        if (out > 0) {
            cout << TAG << "Read a task successfully!" << endl;
            switch (task.task) {
                case (FIND_PARTNER):
                    findPartner(task.pid);
                    break;
                case (TIDE_CHANGE):
                    updateFieldList(task.pid, task.tideRise);
                    break;
                case (MATCH_RESULT):
                    saveResult(task.pid, task.resultLocal, task.resultVisitant);
                    break;
                default:
                    throw runtime_error("Task handler not defined.");
            }
            cout << TAG << "Task completed! Going for a new one" << endl;
        }
    }
}

/**
 * Updates the field list for a tide change.
 *
 * @param fieldPid the pid of the field to update.
 * @param tideRise true if the tide rise or false if the tide fall.
 */
void Manager::updateFieldList(int fieldPid, bool tideRise) {
    freeFields[fieldPidNumberMap[fieldPid]] = !tideRise;
}

/**
 * Removes the players of a team from their respective possible partner lists.
 *
 * @param team the team to erase their partners.
 */
void Manager::removePlayerFromPossiblePartners(Team team) {
    playersPossiblePartners[team.pidPlayer1].erase(find(playersPossiblePartners[team.pidPlayer1].begin(),
                                                        playersPossiblePartners[team.pidPlayer1].end(),
                                                        team.pidPlayer2));
    playersPossiblePartners[team.pidPlayer2].erase(find(playersPossiblePartners[team.pidPlayer2].begin(),
                                                        playersPossiblePartners[team.pidPlayer2].end(),
                                                        team.pidPlayer1));
}

/**
 * Saves the results of the match, and stores the players that played together.
 * Frees the field.
 *
 * @param fieldPid the pid of the field.
 * @param resultLocal the number of sets won by the local team.
 * @param resultVisitant the number of sets won by the visit team.
 */
void Manager::saveResult(int fieldPid, int resultLocal, int resultVisitant) {
    int fieldNumber = fieldPidNumberMap[fieldPid];
    TeamsMatch teamsMatch = teamsOnFields[fieldNumber];
    freeFields[fieldNumber] = true;
    removePlayerFromPossiblePartners(teamsMatch.localTeam);
    removePlayerFromPossiblePartners(teamsMatch.visitTeam);
    playersInGame = playersInGame - 4;
    // TODO: update the points of the players in the shared memory
}

/**
 * Sends a message to a particular player.
 *
 * @param playerPid the pid of the player.
 * @param orgPlayerResponse the message to send.
 */
void Manager::sendMessageToPlayer(int playerPid, OrgPlayerResponse orgPlayerResponse) {
    if (playersPidFifoMap.find(playerPid) == playersPidFifoMap.end()) {
        FifoWrite *f = ResourceHandler::getInstance()->createFifoWrite(FIFO_FILE_PARTNER_RESPONSE + to_string(playerPid));
        int fd = f->openFifo();
        if (fd < 0) {
            stringstream message;
            message << TAG << "Trying to open a fifo to write a response. Fifo couldn't be opened. Error Number: "
                    << strerror(errno) << " " << errno << " Tamaño:" << playersPidFifoMap.size();
            throw runtime_error(message.str());
        }
        playersPidFifoMap.emplace(playerPid, *f);
    }
    FifoWrite fifoWrite = playersPidFifoMap[playerPid];

    cout << TAG << "A partner for process: " << playerPid << "was found!!" << endl;
    cout << TAG << "Trying to write a response" << endl;
    ssize_t out = fifoWrite.writeFifo(static_cast<const void *> (&orgPlayerResponse), sizeof(OrgPlayerResponse));
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
void Manager::sendPlayersToField(TeamsMatch teamsMatch, int column, int row) {
    OrgPlayerResponse orgPlayerResponse = OrgPlayerResponse{column, row, ENUM_PLAY};
    sendMessageToPlayer(teamsMatch.localTeam.pidPlayer1, orgPlayerResponse);
    sendMessageToPlayer(teamsMatch.localTeam.pidPlayer2, orgPlayerResponse);
    sendMessageToPlayer(teamsMatch.visitTeam.pidPlayer1, orgPlayerResponse);
    sendMessageToPlayer(teamsMatch.visitTeam.pidPlayer2, orgPlayerResponse);
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
        auto partnerPid = find(playersPossiblePartners[teamProject->pidPlayer1].begin(),
                                                playersPossiblePartners[teamProject->pidPlayer1].end(), *waitingPlayer);
        // If its the player do the search again from the player on.
        if (*partnerPid == teamProject->pidPlayer1) {
            partnerPid = find(partnerPid, playersPossiblePartners[teamProject->pidPlayer1].end(), *waitingPlayer);
        }
        if (partnerPid != playersPossiblePartners[teamProject->pidPlayer1].end()) {
            teamProject->pidPlayer2 = *partnerPid;
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
    teamProject->pidPlayer1 = waitingTeams[0].pidPlayer1;
    teamProject->pidPlayer2 = waitingTeams[0].pidPlayer1;
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
            teamsOnFields[i] = teamsMatch;
            sendPlayersToField(teamsMatch, i - columns * (i % columns), i % columns);
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
    return waitingPlayers.size() + 2 * waitingTeams.size() + playersInGame + 1 < stadiumSize;
}

/**
 * Picks a random player from the waiting players and send him out of the stadium.
 */
void Manager::removeRandomWaitingPlayer() {
    int randomPlayer = getRandomInt(0, static_cast<int>(waitingPlayers.size()));
    sendMessageToPlayer(waitingPlayers[randomPlayer], OrgPlayerResponse{0, 0, ENUM_LEAVE_STADIUM});
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
            waitingPlayers.erase(find(waitingPlayers.begin(), waitingPlayers.end(), team.pidPlayer1));
            waitingPlayers.erase(find(waitingPlayers.begin(), waitingPlayers.end(), team.pidPlayer2));
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
 * @param playerPid the pid of the player.
 */
void Manager::findPartner(int playerPid) {
    if (playerPlayAllGames(playerPid)) {
        sendMessageToPlayer(playerPid, OrgPlayerResponse{0, 0, ENUM_LEAVE_TOURNAMENT});
    } else {
        Team localTeam = Team{playerPid, 0};
        if (assignPartner(&localTeam)) {
            Team opponentTeam{};
            if (findOpponents(&opponentTeam)) {
                if (assignField(localTeam, opponentTeam)) {
                    waitingPlayers.erase(find(waitingPlayers.begin(), waitingPlayers.end(), localTeam.pidPlayer2));
                    waitingTeams.erase(find(waitingTeams.begin(), waitingTeams.end(), opponentTeam));
                } else {
                    waitingPlayers.erase(find(waitingPlayers.begin(), waitingPlayers.end(), localTeam.pidPlayer2));
                    waitingTeams.push_back(localTeam);
                }
            } else {
                waitingPlayers.erase(find(waitingPlayers.begin(), waitingPlayers.end(), localTeam.pidPlayer2));
                waitingTeams.push_back(opponentTeam);
            }
        } else {
            if (stadiumIsFull()) {
                removeRandomWaitingPlayer();
            }
            waitingPlayers.push_back(playerPid);
            formTeamsAndAssignFields();
        }
    }
}

/**
 * Checks if the given player have played all his games.
 *
 * @param playerPid the player to check.
 * @return true if the player played all his games.
 */
bool Manager::playerPlayAllGames(int playerPid) {
    return totalPlayers - playersPossiblePartners[playerPid].size() >= totalGames;
}

bool Manager::checkTournamentEnd() {
    return count < 100;
}