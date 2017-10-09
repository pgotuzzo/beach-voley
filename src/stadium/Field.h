#ifndef BEACH_VOLEY_FIELD_H
#define BEACH_VOLEY_FIELD_H

#include "../../IPCClasses/Semaforo.h"
#include "../../Logger/Logger.h"
#include "../config/Definitions.h"
#include "../../IPCClasses/fifo/FifoWrite.h"

using namespace std;

struct MatchResult {
    int Team1, Team2;
};

class Field {

public:
    Field() = default;

    Field(string name, Semaforo *entrance, unsigned short entranceId, Semaforo *exit, unsigned short exitId,
          int minGameDurationInMicro, int maxGameDurationInMicro);

    void readyForGames();

    SemaforoInfo getEntry();

    SemaforoInfo getExit();

private:
    string name;
    SemaforoInfo entrance{};
    SemaforoInfo exit{};
    int minGameDurationInMicro{};
    int maxGameDurationInMicro{};
    FifoWrite *taskToManagerFifo;

    void log(string message);

    void waitForPlayers();

    void setResult(TaskRequest *taskRequest);

    void sendResult();

    void releasePlayers();
};

#endif //BEACH_VOLEY_FIELD_H
