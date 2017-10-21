#ifndef BEACH_VOLEY_AUX_PLAYERPROCESS_H
#define BEACH_VOLEY_AUX_PLAYERPROCESS_H


#include "../ipc/Semaforo.h"
#include "../ipc/Pipe.h"
#include "../cons/Definition.h"

using namespace std;

class PlayerProcess {
private:
    string playerName;
    bool leaveTournament = false;
    bool leaveStadium = false;
    Semaforo *tournamentSubscription;
    Semaforo *tournamentStart;
    Semaforo *stadiumEntrance;
    Pipe *pipeToManager;
    Pipe *pipeFromManager;

    void subscribeToTournament();

    void sendFindPartnerRequest();

    OrgPlayerResponse receiveFindPartnerResponse();

    void play();

public:
    PlayerProcess(string playerName, Semaforo *tournamentSubscription, Semaforo *tournamentStart,
                  Semaforo *stadiumEntrance, Pipe *pipeToManager, Pipe *pipeFromManager);

    int start();
};


#endif //BEACH_VOLEY_AUX_PLAYERPROCESS_H
