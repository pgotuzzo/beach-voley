#ifndef BEACH_VOLEY_PLAYER_H
#define BEACH_VOLEY_PLAYER_H

#include <string>
#include "../Definitions.h"
#include "../court/Court.h"

using namespace std;
extern const char* FIFO_FILE_TOURNAMENT;
extern const char* FIFO_FILE_PARTNER_REQUEST;
extern const char* FIFO_FILE_PARTNER_RESPONSE;


class Player {

private:

    string name;
    OrgPlayerResponse *response;
    Court **courts;
    Court getCourt() ;
    SemaforoInfo getSemaforoInfoEntry();
public:
    Player(const string &name, Court **courts);

    void subscribe();
    void partnerRequest();
    void organizatorResponse();
    void removeTmpFile(string basic_string);
    void goToPlayCourt();
    void leaveCourt();

    SemaforoInfo getSemaforoInfoExit();

    void play();
};


#endif //BEACH_VOLEY_PLAYER_H
