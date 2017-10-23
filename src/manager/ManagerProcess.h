#ifndef BEACH_VOLEY_AUX_MANAGERPROCESS_H
#define BEACH_VOLEY_AUX_MANAGERPROCESS_H


#include <map>
#include "../player/Player.h"
#include "SecurityGuard.h"
#include "MatchMaker.h"
#include "../ipc/VectorCompartido.h"
#include "../ipc/LockFile.h"

class ManagerProcess {
private:
    vector<Player> *vPlayers;
    Stadium *stadium;
    SecurityGuard *securityGuard;
    MatchMaker *matchMaker;
    Pipe *taskQueue;
    LockFile *lock;
    VectorCompartido<Player> *scoreBoard;
    int scoreBoardSize;
    int maxMatches;
    int stadiumCapacity;
    bool tournamentEnded;

    TaskRequest receiveTask();

    void onFindPartnerRequest(int playerId);

    void onMatchResultRequest(int fieldId, int localScore, int visitantScore);

    void onTideChangeRequest(bool rise);

    bool isStadiumFull();

    bool isPlayerOutside();

    void refreshScoreBoard(Match match);

public:
    ManagerProcess(vector<Player> *players, Pipe *pipeFromClient, Stadium *stadium, int maxMatches,
                   int stadiumCapacity);

    int start();
};


#endif //BEACH_VOLEY_AUX_MANAGERPROCESS_H
