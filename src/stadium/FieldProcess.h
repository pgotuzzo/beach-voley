#ifndef BEACH_VOLEY_FIELDPROCESS_H
#define BEACH_VOLEY_FIELDPROCESS_H


#include "../ipc/Semaforo.h"
#include "../ipc/Pipe.h"

using namespace std;

class FieldProcess {
private:
    struct MatchResult {
        int localScore;
        int visitantScore;
    };

    const string TAG;
    Pipe *managerQueue;
    Semaforo *entrance;
    Semaforo *exit;
    int playersInField;


    void waitForPlayers();

    MatchResult playMatch();

    void releasePlayers();

    void sendResult(MatchResult matchResult);

public:
    FieldProcess(string name, Pipe *managerQueue, Semaforo *entrance, Semaforo *exit);

    int start();
};


#endif //BEACH_VOLEY_FIELDPROCESS_H
