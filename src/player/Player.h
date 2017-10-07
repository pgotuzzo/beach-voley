#ifndef BEACH_VOLEY_PLAYER_H
#define BEACH_VOLEY_PLAYER_H

#include <string>
#include "../Definitions.h"
#include "../court/Court.h"
#include "../court/Field.h"
#include "../../Logger/Logger.h"

using namespace std;
//extern const char *FIFO_FILE_PARTNER_REQUEST;
//extern const char *FIFO_FILE_PARTNER_RESPONSE;


class Player {

private:
    string name;
    OrgPlayerResponse *response;
    Field *field;
    static const Logger* logger;
    const Semaforo* fieldTurnstile;

    Court getCourt();

    SemaforoInfo getSemaforoInfoEntry();

    void partnerRequest();

    void organizatorResponse();

    void removeTmpFile(string basic_string);

    void goToPlayCourt();

    void leaveCourt();

    void log(string message);

    void enterField();

    SemaforoInfo getSemaforoInfoExit();

public:
    Player(const string &name, Field *field, const Semaforo *fieldTurnstile);

    void play();
};


#endif //BEACH_VOLEY_PLAYER_H
