#ifndef BEACH_VOLEY_AUX_SECURITYGUARD_H
#define BEACH_VOLEY_AUX_SECURITYGUARD_H


#include "../player/Player.h"

class SecurityGuard {
private:
    vector<Player> *vPlayers;

    enum DismissAction {
        TOURNAMENT, STADIUM
    };

    void dismissPlayer(Player player, DismissAction action);

public:
    SecurityGuard(vector<Player> *players);

    void dismissPlayerFromStadium();

    void dismissPlayerFromTournament(int playerId);

    void dismissPlayersWaitingFromTournament();
};


#endif //BEACH_VOLEY_AUX_SECURITYGUARD_H
