#ifndef BEACH_VOLEY_FIELD_H
#define BEACH_VOLEY_FIELD_H

#include "../../IPCClasses/Semaforo.h"
#include "../../Logger/Logger.h"
#include "../config/Definitions.h"
#include "../../IPCClasses/Pipe.h"

using namespace std;

struct MatchResult {
    int Team1, Team2;
};

class Field {

public:
    Field() = default;

    ~Field();

    Field(unsigned short id, string name, Semaforo *entrance, Semaforo *exit, int minGameDurationInMicro,
          int maxGameDurationInMicro, Pipe *taskToManagerPipe);

    void readyForGames();

    SemaforoInfo getEntry();

    SemaforoInfo getExit();

    void releasePlayers();

    void toggleFloodedAndSendNotification();

private:
    unsigned short id;
    string name;
    bool flooded = false;
    bool thereArePlayersToRelease = false;
    SemaforoInfo entrance{};
    SemaforoInfo exit{};
    int minGameDurationInMicro{};
    int maxGameDurationInMicro{};
    Pipe *taskToManagerPipe;

    void log(string message);

    void waitForPlayers();

    void setResult(TaskRequest *taskRequest);

    void sendResult();
};

#endif //BEACH_VOLEY_FIELD_H
