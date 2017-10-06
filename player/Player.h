#ifndef BEACH_VOLEY_PLAYER_H
#define BEACH_VOLEY_PLAYER_H

#include <string>

using namespace std;

class Player {
private:
    string name;
public:
    Player(const string &name);

    void subscribe();
    void partnerRequest();
};


#endif //BEACH_VOLEY_PLAYER_H
