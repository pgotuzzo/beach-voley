#ifndef BEACH_VOLEY_STADIUM_H
#define BEACH_VOLEY_STADIUM_H

#include <vector>
#include "Field.h"

class Stadium {
private:
    int rows;
    int columns;
    vector<Field> fields;

public:
    Stadium(int columns, int rows);

    Field getField(int column, int row);

    int getColumns();

    int getRows();
};


#endif //BEACH_VOLEY_STADIUM_H
