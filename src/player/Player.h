#ifndef BEACH_VOLEY_PLAYER_H
#define BEACH_VOLEY_PLAYER_H

#include <string>
#include "../Definitions.h"
#include "../court/Court.h"
#include "../court/Field.h"
#include "../../Logger/Logger.h"
#include "../../IPCClasses/fifo/FifoRead.h"
#include "PartnerRequester.h"

using namespace std;


class Player {

private:
    string name;
    Field *field;
    const Semaforo *fieldTurnstile;
    OrgPlayerResponse *response;
    PartnerRequester *requester;

    Court getCourt();

    SemaforoInfo getSemaforoInfoEntry();

    void partnerRequest();

    void goToPlayCourt();

    void leaveCourt();

    void log(string message);

    void enterField();

    void leaveField();

    SemaforoInfo getSemaforoInfoExit();

public:
    Player(const string &name, Field *field, const Semaforo *fieldTurnstile);

    void play();

};


#endif //BEACH_VOLEY_PLAYER_H
