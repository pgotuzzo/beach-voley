#ifndef BEACH_VOLEY_DEFINITIOS_H
#define BEACH_VOLEY_DEFINITIOS_H

#include <iostream>
#include "court/Court.h"

using namespace std;

typedef enum PlayerAction {
    ENUM_PLAY, ENUM_LEAVE_STADIUM, ENUM_LEAVE_TOURNAMENT
};

typedef struct OrgPlayerResponse {
    int column;
    int row;
    PlayerAction playerAction;

    string show() {
        return string("Column: ") + to_string(column) + string(" row: ") + to_string(row) + string(" player action: ") +
               to_string(playerAction);
    }

};

#endif //BEACH_VOLEY_DEFINITIOS_H
