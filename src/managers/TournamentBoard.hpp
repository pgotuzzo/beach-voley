#ifndef BEACH_VOLEY_TOURNAMENTBOARD_HPP
#define BEACH_VOLEY_TOURNAMENTBOARD_HPP

#include "../../IPCClasses/VectorCompartido.h"
#include "../../IPCClasses/LockFile.h"

class TournamentBoard {
private:
    VectorCompartido<int> *pidsTable;
    VectorCompartido<int> *pointsTable;
    LockFile *lockForSharedVectors;
    unsigned long numberOfPlayers;

public:
    TournamentBoard(VectorCompartido<int> *pidsTable, VectorCompartido<int> *pointsTable,
                    LockFile *lockForSharedVectors, unsigned long numberOfPlayers);

    void printTableValues();
};


#endif //BEACH_VOLEY_TOURNAMENTBOARD_HPP
