#ifndef BEACH_VOLEY_FIELD_H
#define BEACH_VOLEY_FIELD_H


#include <string>
#include "../ipc/Pipe.h"

using namespace std;

class Field {
private:
    int id;
    string name;

public:
    Field();

    Field(int id, string name);

    bool operator==(const Field &field) const;

    int getId();

};


#endif //BEACH_VOLEY_FIELD_H
