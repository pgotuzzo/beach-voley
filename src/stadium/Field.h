#ifndef BEACH_VOLEY_FIELD_H
#define BEACH_VOLEY_FIELD_H

#include "../../IPCClasses/Semaforo.h"
#include "../../Logger/Logger.h"

using namespace std;

struct MatchResult{
    int Team1, Team2;
};

class Field {

public:
    Field();

    Field(string name, Semaforo *entrance, unsigned short entranceId, Semaforo *exit, unsigned short exitId);

    SemaforoInfo getEntry();

    SemaforoInfo getExit();

    void waitForPlayers();
private:
    string name;
    SemaforoInfo entrance;
    SemaforoInfo exit;

    void log(string message);

    void play();

    MatchResult getResult();
};

#endif //BEACH_VOLEY_FIELD_H
