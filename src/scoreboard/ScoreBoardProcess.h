#ifndef BEACH_VOLEY_SCOREBOARDPROCESS_H
#define BEACH_VOLEY_SCOREBOARDPROCESS_H

#include "../ipc/LockFile.h"
#include "../ipc/VectorCompartido.h"
#include "../player/Player.h"

class ScoreBoardProcess {
private:
    LockFile *lock;
    VectorCompartido<Player> *vPlayers;
    int playerCount;
    bool quit;

    void show();

public:
    ScoreBoardProcess(int playerCount);

    int start();
};


#endif //BEACH_VOLEY_SCOREBOARDPROCESS_H
