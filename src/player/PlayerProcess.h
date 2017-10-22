#ifndef BEACH_VOLEY_AUX_PLAYERPROCESS_H
#define BEACH_VOLEY_AUX_PLAYERPROCESS_H


#include "../ipc/Semaforo.h"
#include "../ipc/Pipe.h"
#include "../cons/Definition.h"
#include "../stadium/Stadium.h"

using namespace std;

class PlayerProcess {
private:
    string playerName;
    bool leaveTournament = false;
    bool leaveStadium = false;
    Semaforo *tournamentSubscription;
    Semaforo *tournamentStart;
    Semaforo *stadiumEntrance;
    Semaforo *fieldEntrance;
    Semaforo *fieldExit;
    Pipe *pipeToManager;
    Pipe *pipeFromManager;
    Stadium *stadium;

    void subscribeToTournament();

    void sendFindPartnerRequest();

    OrgPlayerResponse receiveFindPartnerResponse();

    void play();

    void goToField(int fieldId);

public:
    PlayerProcess(string playerName, Semaforo *tournamentSubscription, Semaforo *tournamentStart,
                  Semaforo *stadiumEntrance, Semaforo *fieldEntrance, Semaforo *fieldExit, Pipe *pipeToManager,
                  Pipe *pipeFromManager, Stadium *stadium);

    int start();
};


#endif //BEACH_VOLEY_AUX_PLAYERPROCESS_H
