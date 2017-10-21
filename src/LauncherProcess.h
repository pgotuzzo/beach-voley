#ifndef BEACH_VOLEY_AUX_LAUNCHERPROCESS_H
#define BEACH_VOLEY_AUX_LAUNCHERPROCESS_H


#include "ipc/Semaforo.h"

class LauncherProcess {
private:
    unsigned short int playersAmount;
    Semaforo *tournamentSubscription;
    Semaforo *tournamentStart;

    void openSubscriptions();

    void startTournament();
public:
    LauncherProcess(unsigned short int playersAmount, Semaforo *tournamentSubscription, Semaforo *tournamentStart);

    int start();
};


#endif //BEACH_VOLEY_AUX_TOURNAMENT_H
