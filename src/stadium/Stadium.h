#ifndef BEACH_VOLEY_STADIUM_H
#define BEACH_VOLEY_STADIUM_H

#include <vector>
#include "Field.h"

using namespace std;

class Stadium {
private:
    int rows;
    int columns;
    vector<Field> vFields;

public:
    Stadium(int rows, int columns, vector<Field> vFields);

    Field* getFieldById(int fieldId);

    int getFieldIndex(int fieldId);
};


#endif //BEACH_VOLEY_STADIUM_H
