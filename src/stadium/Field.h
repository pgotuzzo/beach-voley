#ifndef BEACH_VOLEY_FIELD_H
#define BEACH_VOLEY_FIELD_H


#include <string>
#include "../ipc/Pipe.h"
#include "../cons/Definition.h"

using namespace std;

class Field {
public:
    enum State {
        FREE, FULL, FLOODED
    };

private:
    int id;
    int semId;
    string name;
    State state;
    Match match;

public:
    Field();

    Field(int id, int semId, string name);

    bool operator==(const Field &field) const;

    int getId();

    int getSemId();

    string getName();

    State getState();

    void setState(State state);

    Match getMatch();

    void setMatch(Match match);

};


#endif //BEACH_VOLEY_FIELD_H
