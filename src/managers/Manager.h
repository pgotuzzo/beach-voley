#ifndef BEACH_VOLEY_MANAGER_H
#define BEACH_VOLEY_MANAGER_H

#include <vector>
#include "../player/Player.h"
#include "../../IPCClasses/VectorCompartido.h"
#include "../config/Config.h"
#include "../../IPCClasses/LockFile.h"

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

    struct FieldInformation {
        bool isFree = true;
        Team localTeam{};
        Team visitTeam{};
    };

    // The number of players in the tournament
    unsigned long playersInTournament;
    // The number of games in play
    int gamesInPlay = 0;
    // If the tournament already start, so there is more than 10 players inside the stadium
    bool tournamentStart = false;
    // The maximum number of matches a player can play before leaving the tournament
    unsigned int totalMatchesPerPlayer;
    // The maximum number of players inside the stadium
    unsigned int stadiumSize;
    // Number of players inside the stadium before the tournament starts
    int playersToStartTournament;

    // A vector that represents if the field is free and if not what teams are in it.
    vector<FieldInformation> fieldsInformation;

    // The players that ended the game and are returning to the manager for a new match
    vector<int> playersReturningFromField;
    // The players waiting for a match
    vector<int> waitingPlayers;
    // The teams waiting for a match
    vector<Team> waitingTeams;

    // The number of games that played every player
    vector<int> playersGames;
    // For every player there is a vector that represent if every other player in the tournament is an available
    // partner for him
    vector<vector<bool>> playersAvailablePartners;

    // The pipe where the manager receives the tasks
    Pipe *receiveTaskPipe;
    // A vector with the pipe where the manager send responses for the players
    vector<Pipe *> playersIdPipeMap;

    // Lock to write or read in the shared memory
    LockFile *lockForSharedMemory;
    // A shared vector with the points for every player
    VectorCompartido<int> *pointsTable;

    void sendMessageToPlayer(int playerId, OrgPlayerResponse orgPlayerResponse);

    void findMatch(int playerPid);
    bool assignPartner(Team *teamProject);
    bool assignField(Team localTeam, Team visitTeam);
    void sendPlayersToField(unsigned short fieldId);

    bool stadiumIsFull();
    void removeRandomWaitingPlayer();

    int countAvailablePartners(int id);
    void removePlayersThatCantPlay();
    void removePlayerFromAllAvailablePartners(int playerId);
    void removePlayersFromAvailablePartners(Team team);

    void updateFieldList(int fieldPid, bool tideRise);
    void saveResult(int fieldPid, int resultLocal, int resultVisitant);
public:
    Manager(unsigned int totalFields, unsigned int capacity, unsigned int matches, unsigned int players,
            LockFile *lockForSharedMemory, VectorCompartido<int> *pointsTable, Pipe *receiveTaskPipe,
            Pipe *playersIdPipeDir, int playersToStartTournament);

    void receiveTask();
};


#endif //BEACH_VOLEY_MANAGER_H
