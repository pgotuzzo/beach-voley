#ifndef BEACH_VOLEY_FIELD_H
#define BEACH_VOLEY_FIELD_H

#include "../../IPCClasses/Semaforo.h"
#include "../config/Definitions.h"
#include "../../IPCClasses/Pipe.h"

using namespace std;

class Field {
public:
    ~Field() = default;

    Field(unsigned short id, Semaforo *entranceSemaphore, Semaforo *exitSempahore, Pipe *taskToManagerPipe,
          int minGameDurationInMicro, int maxGameDurationInMicro);

    void startPlayingGames();
    void releasePlayersAndEndGame();
    void toggleFloodedAndSendNotification();

private:
    unsigned short id;
    bool flooded = false;
    bool inGame = false;
    int minGameDurationInMicro;
    int maxGameDurationInMicro;

    // semaphore that represent the entranceSemaphore to the field where players will announce that they arrive to the field
    Semaforo *entranceSemaphore;
    // semaphore that represent the exitSemahore of the field where players will wait till the game ends
    Semaforo *exitSemahore;
    // pipe to send task to the manager
    Pipe *tasksToManagerPipe;

    void waitForPlayersAndStartGame();
    void sendResult();
    void logMessage(const string &message);
};

#endif //BEACH_VOLEY_FIELD_H
