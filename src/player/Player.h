#ifndef BEACH_VOLEY_PLAYER_H
#define BEACH_VOLEY_PLAYER_H

#include <string>
#include "../Definitions.h"
#include "../court/Court.h"
#include "../court/Field.h"
#include "../../IPCClasses/FifoWrite.h"

using namespace std;
//extern const char *FIFO_FILE_PARTNER_REQUEST;
//extern const char *FIFO_FILE_PARTNER_RESPONSE;


class Player {

private:
    string name;
    OrgPlayerResponse response;
    Field *field;

    Court getCourt();

    SemaforoInfo getSemaforoInfoEntry();

    void partnerRequest();

    void organizatorResponse();

    void removeTmpFile(string basic_string);

    void goToPlayCourt();

    void leaveCourt();

    SemaforoInfo getSemaforoInfoExit();

    FifoWrite *fifo;

public:
    Player(const string &name, Field *field);

    void play();

    void log(string message);
};


#endif //BEACH_VOLEY_PLAYER_H
