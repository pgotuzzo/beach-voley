#ifndef BEACH_VOLEY_COURT_H
#define BEACH_VOLEY_COURT_H

#include "../../IPCClasses/Semaforo.h"

using namespace std;

class Court {
private:
    string name;
    SemaforoInfo entrance;
    SemaforoInfo exit;

    void log(string message);

    void waitForPlayers();

    void play();

public:
    Court();

    Court(string name, Semaforo *entrance, unsigned short entranceId, Semaforo *exit, unsigned short exitId);

    SemaforoInfo getEntry();

    SemaforoInfo getExit();
};

#endif //BEACH_VOLEY_COURT_H
