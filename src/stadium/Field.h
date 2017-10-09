#ifndef BEACH_VOLEY_FIELD_H
#define BEACH_VOLEY_FIELD_H

#include "../../IPCClasses/Semaforo.h"
#include "../../Logger/Logger.h"

using namespace std;

struct MatchResult {
    int Team1, Team2;
};

class Field {

public:
    Field() = default;

    Field(string name, Semaforo *entrance, unsigned short entranceId, Semaforo *exit, unsigned short exitId,
          int minGameDurationInMicro, int maxGameDurationInMicro);

    SemaforoInfo getEntry();

    SemaforoInfo getExit();

    void waitForPlayers();

private:
    string name;
    SemaforoInfo entrance{};
    SemaforoInfo exit{};
    int minGameDurationInMicro{};
    int maxGameDurationInMicro{};

    void log(string message);

    void play();

    MatchResult getResult();
};

#endif //BEACH_VOLEY_FIELD_H
