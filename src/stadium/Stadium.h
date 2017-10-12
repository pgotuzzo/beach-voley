#ifndef BEACH_VOLEY_STADIUM_H
#define BEACH_VOLEY_STADIUM_H

#include <vector>
#include "Field.h"
#include "../../IPCClasses/Pipe.h"

class Stadium {
private:
    int rows;
    int columns;
    vector<Field> fields;

public:
    Stadium(int columns, int rows, int minGameDurationInMicro, int maxGameDurationInMicro, Pipe *managerReceive);

    vector<int> initStadium();

    Field getField(int id);

    int getColumns();

    int getRows();
};


#endif //BEACH_VOLEY_STADIUM_H
