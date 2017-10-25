#ifndef BEACH_VOLEY_FIELDPROCESS_H
#define BEACH_VOLEY_FIELDPROCESS_H


#include <sstream>
#include "../ipc/Semaforo.h"
#include "../ipc/Pipe.h"

using namespace std;

class FieldProcess {
private:
    enum State {
        WAITING_FOR_PLAYERS, GAME_IN_PROGRESS, NOTIFYING_RESULT
    };

    struct MatchResult {
        int localScore;
        int visitantScore;

        string toString() {
            stringstream stream;
            stream << "Resultado del partido:" << endl
                   << "\t\tLocal: " << localScore << " - " << visitantScore << ":Visitante";
            return stream.str();
        }
    };

    const string TAG;
    int semId;
    Pipe *managerQueue;
    Semaforo *entrance;
    Semaforo *exit;
    int playersInField;

    bool isFlood;
    bool tideChanged;

    bool abort;
    State state;

    void waitForPlayers();

    MatchResult playMatch();

    void releasePlayers();

    void sendResult(MatchResult matchResult);

    void notifyTideChange();

public:
    FieldProcess(string name, Pipe *managerQueue, int semId, Semaforo *entrance, Semaforo *exit);

    void toggleTide();

    int start();
};


#endif //BEACH_VOLEY_FIELDPROCESS_H
