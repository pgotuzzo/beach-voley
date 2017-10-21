#ifndef BEACH_VOLEY_AUX_MATCHMAKER_H
#define BEACH_VOLEY_AUX_MATCHMAKER_H


#include "../player/Player.h"

class MatchMaker {
private:
    vector<Player> *vPlayers;

public:
    MatchMaker(vector<Player> *players);

    bool findPartnerAvailable(int playerId);
};


#endif //BEACH_VOLEY_AUX_MATCHMAKER_H
