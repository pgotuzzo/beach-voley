#ifndef BEACH_VOLEY_MANAGER_H
#define BEACH_VOLEY_MANAGER_H

#include <map>
#include <vector>
#include "../player/Player.h"
#include "../../IPCClasses/VectorCompartido.h"
#include "../config/Config.h"

using namespace std;

class Manager {
private:
    struct Team {
        int idPlayer1;
        int idPlayer2;

        bool operator==(const Team &compTeam) const {
            return compTeam.idPlayer1 == idPlayer1 and compTeam.idPlayer2 == idPlayer2;
        }
    };

    struct TeamsMatch {
        Team localTeam;
        Team visitTeam;
    };

    struct MatchResult {
        TeamsMatch teamsInMatch;
        int localScore;
        int visitScore;
    };

    vector<MatchResult> matchHistory;
    // TODO: initilize this with the id of all players same time as playersIdPipeMap
    VectorCompartido<int> *idsTable;
    map<int, int> idToVectorIndexMap;
    VectorCompartido<int> *pointsTable;
    LockFile *lockForSharedVectors;
    int count = 0;
    unsigned int stadiumSize;
    unsigned int totalGames;
    unsigned int playersInGame = 0;
    unsigned long totalPlayersInTournament;
    unsigned long initialPlayersInTournament;
    // TODO: all ids, playersIdPipeMap will have one of this per player
    map<int, Pipe *> playersIdPipeMap;
    vector<TeamsMatch> teamsOnFields;
    vector<Team> waitingTeams;
    vector<int> waitingPlayers;
    map<int, vector<int>> playersPossiblePartners;
    vector<bool> freeFields;
    Pipe *receiveTaskPipe;
    unsigned int rows, columns;

    void findPartner(int playerPid);

    void receiveTask();

    bool checkTournamentEnd();

    void updateFieldList(int fieldPid, bool tideRise);

    void saveResult(int fieldPid, int resultLocal, int resultVisitant);

    void sendPlayersToField(TeamsMatch teamsMatch, int fieldId);

    void sendMessageToPlayer(int playerId, OrgPlayerResponse orgPlayerResponse);

    bool assignPartner(Team *teamProject);

    bool findOpponents(Team *teamProject);

    bool assignField(Team localTeam, Team visitTeam);

    bool stadiumIsFull();

    void removeRandomWaitingPlayer();

    void removePlayersFromPossiblePartners(Team team);

    bool playerPlayAllGamesOrHasNoPossiblePartner(int playerId);

    void removePlayerFromPossiblePartner(int targetPlayer, int playerToRemove);

    bool removePlayersThatCantPlay();
public:
    Manager(TournamentParams tournamentParams, VectorCompartido<int> *idsTable, VectorCompartido<int> *pointsTable,
            LockFile *lockForSharedVectors, Pipe *receiveTaskPipe, map<int, Pipe *> playersIdPipeMap);

    void initManager();

    void removePlayerFromAllPossiblePartners(int playerId);
};


#endif //BEACH_VOLEY_MANAGER_H
