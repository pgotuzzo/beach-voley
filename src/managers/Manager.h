#ifndef BEACH_VOLEY_MANAGER_H
#define BEACH_VOLEY_MANAGER_H

#include <map>
#include <vector>
#include "../../IPCClasses/fifo/FifoRead.h"
#include "../../IPCClasses/fifo/FifoWrite.h"
#include "../player/Player.h"
#include "../../IPCClasses/VectorCompartido.h"

using namespace std;

class Manager {
private:
    struct Team{
        int idPlayer1;
        int idPlayer2;

        bool operator==(const Team &compTeam) const {
            return compTeam.idPlayer1 == idPlayer1 and compTeam.idPlayer2 == idPlayer2;
        }
    };

    struct TeamsMatch{
        Team localTeam;
        Team visitTeam;
    };

    struct MatchResult{
        TeamsMatch teamsInMatch;
        int localScore;
        int visitScore;
    };

    vector<MatchResult> matchHistory;
    // TODO: initilize this with the id of all players same time as playersIdFifoMap
    VectorCompartido<int> *idsTable;
    map<int, int> idToVectorIndexMap;
    VectorCompartido<int> *pointsTable;
    LockFile *lockForSharedVectors;
    int count = 0;
    unsigned int stadiumSize;
    unsigned int totalGames;
    unsigned int playersInGame = 0;
    // TODO: initilize this with the size of the vector of all the players same time as shared mem
    unsigned long totalPlayers;
    // TODO: all pids, playersIdFifoMap will have one of this per player
    map<int, FifoWrite> playersIdFifoMap;
    // TODO: initialize this map with the pids of the fields associated with an index from 0 to colums*rows
    map<int, int> fieldPidNumberMap;
    vector<TeamsMatch> teamsOnFields;
    vector<Team> waitingTeams;
    vector<int> waitingPlayers;
    // TODO: initialize this map with the ids of the players as keys an a vector of the players
    // ids (including them for simplicity).
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

    void sendMessageToPlayer(int playerId, OrgPlayerResponse orgPlayerResponse);

    bool assignPartner(Team *teamProject);

    bool findOpponents(Team *teamProject);

    bool assignField(Team localTeam, Team visitTeam);

    bool stadiumIsFull();

    void removeRandomWaitingPlayer();

    void formTeamsAndAssignFields();

    void removePlayerFromPossiblePartners(Team team);

    bool playerPlayAllGames(int playerId);
public:
    Manager(unsigned int rows, unsigned int columns, unsigned int stadiumSize, unsigned int totalGames,
            VectorCompartido<int> *pidsTable, VectorCompartido<int> *pointsTable, LockFile *lockForSharedVectors);

    void initManager();
};


#endif //BEACH_VOLEY_MANAGER_H
