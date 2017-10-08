#ifndef BEACH_VOLEY_FIELD_H
#define BEACH_VOLEY_FIELD_H

#include "../../IPCClasses/Semaforo.h"
#include "../../Logger/Logger.h"

using namespace std;

class Field {
private:
    string name;
    SemaforoInfo entrance;
    SemaforoInfo exit;

    void log(string message);

    void play();

public:
    Field();

    Field(string name, Semaforo *entrance, unsigned short entranceId, Semaforo *exit, unsigned short exitId);

    SemaforoInfo getEntry();

    SemaforoInfo getExit();

    void waitForPlayers();
};

#endif //BEACH_VOLEY_FIELD_H
