#ifndef BEACH_VOLEY_PLAYER_H
#define BEACH_VOLEY_PLAYER_H

#include <string>
#include "../config/Definitions.h"
#include "../stadium/Field.h"

using namespace std;


class Player {

private:
    int id;

    const Semaforo *stadiumTurnstile;

    const Semaforo *entranceToFields;
    const Semaforo *exitFromFields;

    Pipe *receiveResponsesPipe;
    Pipe *sendRequestPipe;

    OrgPlayerResponse partnerRequest();
    void goToPlayGame(unsigned short  fieldId);

    void enterStadium();
    void leaveStadium();

    void logMessage(const string &message);
public:
    Player(int id, const Semaforo *entranceToFields, const Semaforo *exitFromFields,
           const Semaforo *stadiumTurnstile, Pipe *receiveResponsesPipe, Pipe *sendRequestPipe);

    void play();
};


#endif //BEACH_VOLEY_PLAYER_H
