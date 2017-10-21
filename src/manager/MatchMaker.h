#ifndef BEACH_VOLEY_AUX_MATCHMAKER_H
#define BEACH_VOLEY_AUX_MATCHMAKER_H


#include "../player/Player.h"

class MatchMaker {
private:
    struct Team {
        bool ready;
        int firstPlayerId;
        int secondPlayerId;
    };

    struct Match {
        Team local;
        Team visitant;
    };

    vector<Player> *vPlayers;

    Team local;
    Team visitant;
    vector<Match> matchesWaitingForField;

public:
    MatchMaker(vector<Player> *players);

    void releasePlayersWithoutMatch();

    bool findPartnerAvailable(int playerId);

    void checkFieldAssignation();
};


#endif //BEACH_VOLEY_AUX_MATCHMAKER_H
