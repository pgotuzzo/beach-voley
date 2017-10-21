#ifndef BEACH_VOLEY_AUX_MANAGERPROCESS_H
#define BEACH_VOLEY_AUX_MANAGERPROCESS_H


#include <map>
#include "../player/Player.h"
#include "SecurityGuard.h"
#include "MatchMaker.h"

class ManagerProcess {
private:
    vector<Player> *vPlayers;
    SecurityGuard *securityGuard;
    MatchMaker *matchMaker;
    Pipe *taskQueue;
    int maxMatches;
    int stadiumCapacity;
    bool tournamentEnded;

    TaskRequest receiveTask();

    void onFindPartnerRequest(int playerId);

    void onMatchResultRequest(int fieldId, int localScore, int visitantScore);

    void onTideChangeRequest(bool rise);

    bool isStadiumFull();

public:
    ManagerProcess(vector<Player> *players, Pipe *pipeFromClient, int maxMatches, int stadiumCapacity);

    int start();
};


#endif //BEACH_VOLEY_AUX_MANAGERPROCESS_H
