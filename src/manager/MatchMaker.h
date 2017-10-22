#ifndef BEACH_VOLEY_AUX_MATCHMAKER_H
#define BEACH_VOLEY_AUX_MATCHMAKER_H


#include "../player/Player.h"
#include "../stadium/Stadium.h"

class MatchMaker {
private:
    vector<Player> *vPlayers;
    Stadium *stadium;
    Team local;
    Team visitant;
    vector<Match> matchesWaitingForField;

    void assignField(int fieldId);

    void sendPlayerToField(int fieldId, int playerId);

public:
    MatchMaker(vector<Player> *players, Stadium *stadium);

    void releasePlayersWithoutMatch();

    bool findPartnerAvailable(int playerId);

    void checkFieldAssignation();
};


#endif //BEACH_VOLEY_AUX_MATCHMAKER_H
