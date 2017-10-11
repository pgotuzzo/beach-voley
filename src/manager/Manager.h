#ifndef BEACH_VOLEY_MANAGER_H
#define BEACH_VOLEY_MANAGER_H

#include <map>
#include <vector>
#include "../../IPCClasses/fifo/FifoRead.h"
#include "../../IPCClasses/fifo/FifoWrite.h"
#include "../player/Player.h"

using namespace std;

class Manager {
private:
    struct Team{
        int pidPlayer1;
        int pidPlayer2;

        bool operator==(const Team &compTeam) const {
            return compTeam.pidPlayer1 == pidPlayer1 and compTeam.pidPlayer2 == pidPlayer2;
        }
    };

    struct TeamsMatch{
        Team localTeam;
        Team visitTeam;
    };

    int count = 0;
    unsigned int stadiumSize;
    unsigned int totalGames;
    unsigned int playersInGame = 0;
    // TODO: initilize this with the size of the vector of all the players
    unsigned long totalPlayers;
    map<int, FifoWrite> playersPidFifoMap;
    // TODO: initialize this map with the pids of the fields associated with an index from 0 to colums*rows
    map<int, int> fieldPidNumberMap;
    vector<TeamsMatch> teamsOnFields;
    vector<Team> waitingTeams;
    vector<int> waitingPlayers;
    // TODO: initialize this map with the pids of the players as keys an a vector of the players
    // pids (including them for simplicity).
    map<int, vector<int>> playersPossiblePartners;
    vector<bool> freeFields;
    FifoRead *receiveTaskFifo;
    int rows, columns;

    void findPartner(int playerPid);

    void receiveTask();
    
    bool checkTournamentEnd();

    void updateFieldList(int fieldPid, bool tideRise);

    void saveResult(int fieldPid, int resultLocal, int resultVisitant);

    void sendPlayersToField(TeamsMatch teamsMatch, int column, int row);

    void sendMessageToPlayer(int playerPid, OrgPlayerResponse orgPlayerResponse);

    bool assignPartner(Team *teamProject);

    bool findOpponents(Team *teamProject);

    bool assignField(Team localTeam, Team visitTeam);

    bool stadiumIsFull();

    void removeRandomWaitingPlayer();

    void formTeamsAndAssignFields();

    void removePlayerFromPossiblePartners(Team team);

    bool playerPlayAllGames(int playerPid);
public:
    Manager(unsigned int rows, unsigned int columns, unsigned int stadiumSize, unsigned int totalGames);

    void initManager();
};


#endif //BEACH_VOLEY_MANAGER_H
