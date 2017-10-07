#include <sstream>
#include "Field.h"

Field::Field(int columns, int rows) {
    this->columns = columns;
    this->rows = rows;
    this->courts = vector<Court>(rows * columns);

    string nameEntrance = "field_entrance";
    Semaforo *entrance = new Semaforo("field_entrance", 0, columns * rows);
    Semaforo *exit = new Semaforo("field_exit", 0, columns * rows);
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++) {
            stringstream name;
            name << "[" << i << ", " << j << "}";
            auto id = (unsigned short) (i * rows + j);
            courts[i * rows + j] = Court(name.str(), entrance, id, exit, id);
        }
    }
}

Court Field::getCourt(int column, int row) {
    return courts[column * rows + row];
}
