#ifndef BEACH_VOLEY_FIELD_H
#define BEACH_VOLEY_FIELD_H


#include <vector>
#include "Court.h"

class Field {
private:
    int rows;
    int columns;
    vector<Court> courts;

public:
    Field(int columns, int rows);

    Court getCourt(int column, int row);
};


#endif //BEACH_VOLEY_FIELD_H
