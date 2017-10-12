#include <iostream>
#include "TournamentBoard.hpp"

/**
 * Prints the content of the pids table and the points table in the standard output.
 */
void TournamentBoard::printTableValues() {
    std::cout << "Pid\t | Puntos" << std::endl;
    lockForSharedVectors->tomarLock();
    for (unsigned int i = 0; i < numberOfPlayers; ++i) {
        std::cout << pidsTable->leer(i) << "\t | " << pointsTable->leer(i);
    }
    lockForSharedVectors->liberarLock();
}

TournamentBoard::TournamentBoard(VectorCompartido<int> *pidsTable, VectorCompartido<int> *pointsTable,
                                 LockFile *lockForSharedVectors) : pidsTable(pidsTable), pointsTable(pointsTable),
                                                                   lockForSharedVectors(lockForSharedVectors) {
}
