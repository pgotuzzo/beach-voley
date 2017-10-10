#ifndef BEACH_VOLEY_PLAYER_H
#define BEACH_VOLEY_PLAYER_H

#include <string>
#include "../config/Definitions.h"
#include "../stadium/Field.h"
#include "../stadium/Stadium.h"
#include "../../Logger/Logger.h"
#include "../../IPCClasses/fifo/FifoRead.h"
#include "PartnerRequester.h"

using namespace std;


class Player {

private:
    string name;
    Stadium *stadium;
    const Semaforo *stadiumTurnstile;
    OrgPlayerResponse response;
    PartnerRequester *requester;

    Field getField();

    SemaforoInfo getSemaforoInfoEntry();

    void partnerRequest();

    void goToPlayGame();

    void leaveField();

    void log(string message);

    void enterStadium();

    void leaveStadium();

    SemaforoInfo getSemaforoInfoExit();

public:
    Player(const string &name, Stadium *stadium, const Semaforo *stadiumTurnstile);

    void init();

    void play();

};


#endif //BEACH_VOLEY_PLAYER_H
