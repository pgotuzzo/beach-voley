#ifndef BEACH_VOLEY_DEFINITIOS_H
#define BEACH_VOLEY_DEFINITIOS_H

#include <iostream>
#include <sstream>

using namespace std;

enum PlayerAction {
    ENUM_PLAY, ENUM_LEAVE_STADIUM, ENUM_LEAVE_TOURNAMENT
};

struct OrgPlayerResponse {
    int column;
    int row;
    PlayerAction playerAction;

    string show() {
        stringstream message;
        message << "Court: (" << column << ", " << row << ") " << endl << "Action: " << playerAction;
        return message.str();
    }

};

#endif //BEACH_VOLEY_DEFINITIOS_H
