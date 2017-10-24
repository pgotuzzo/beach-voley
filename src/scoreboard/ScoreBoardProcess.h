#ifndef BEACH_VOLEY_SCOREBOARDPROCESS_H
#define BEACH_VOLEY_SCOREBOARDPROCESS_H

#include "../ipc/LockFile.h"
#include "../player/Player.h"
#include "../ipc/VectorCompartido.h"

class ScoreBoardProcess {
private:
    LockFile *lock;
    VectorCompartido<PlayerStats> *vPlayers;
    bool quit;

    void show();

public:
    ScoreBoardProcess();

    int start();
};


#endif //BEACH_VOLEY_SCOREBOARDPROCESS_H
