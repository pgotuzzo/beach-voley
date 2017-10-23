#ifndef BEACH_VOLEY_AUX_PLAYER_H
#define BEACH_VOLEY_AUX_PLAYER_H


#include "../ipc/Semaforo.h"
#include "../ipc/Pipe.h"
#include "../cons/Definition.h"

using namespace std;

class Player {
public:
    enum State {
        OUTSIDE, WAITING_FOR_PARTNER, PARTNER_ASSIGNED, READY_FOR_MATCH
    };

private:
    int id;
    string name;
    State state;
    Pipe *pipe;
    vector<int> potentialPartners;
    int matchesCount = 0;
    int points;

public:

    Player();

    Player(int id, string name, Pipe *pipe);

    bool operator==(const Player &player) const;

    int getId();

    string getName();

    void setState(State state);

    State getState();

    Pipe *getPipe();

    void addPotentialPartner(int id);

    vector<int> getPotentialPartners();

    void removePotentialPartner(int id);

    int getMatchesCount();

    void increaseMatchesCount();

    int getPoints();

    void increasePoints(int amount);

    void setPoints(int points);

};


#endif //BEACH_VOLEY_AUX_PLAYER_H
