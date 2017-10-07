#include <sstream>
#include "Field.h"
#include "../Constants.h"

Field::Field(int columns, int rows) {
    this->columns = columns;
    this->rows = rows;
    this->courts = vector<Court>(rows * columns);

    Semaforo *entrance = new Semaforo(SEM_FILE_COURT_ENTRANCE, 0, columns * rows);
    Semaforo *exit = new Semaforo(SEM_FILE_COURT_EXIT, 0, columns * rows);
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

int Field::getColumns() {
    return columns;
}

int Field::getRows() {
    return rows;
}
