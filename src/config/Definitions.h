#ifndef BEACH_VOLEY_DEFINITIOS_H
#define BEACH_VOLEY_DEFINITIOS_H

#include <iostream>
#include <sstream>

using namespace std;

enum PlayerAction {
    ENUM_PLAY, ENUM_LEAVE_STADIUM, ENUM_LEAVE_TOURNAMENT
};

struct OrgPlayerResponse {
    unsigned short fieldId;
    PlayerAction playerAction;

    string show() {
        stringstream message;
        message << "Field: (" << fieldId << ") " << endl << "Action: " << playerAction;
        return message.str();
    }

};

enum TaskType {
    FIND_PARTNER, TIDE_CHANGE, MATCH_RESULT
};

struct TaskRequest {
    int id;
    int resultLocal;
    int resultVisitant;
    bool tideRise;
    TaskType task;

    string show() {
        return string("id: ") + to_string(id) + string(" resultLocal: ") + to_string(resultLocal) +
               string(" resultVisitant: ") +
               to_string(resultVisitant) + string(" task: ") + to_string(task);
    }
};

#endif //BEACH_VOLEY_DEFINITIOS_H
