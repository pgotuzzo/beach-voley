#include <sstream>
#include "Stadium.h"
#include "../config/Constants.h"
#include "../../util/ResourceHandler.h"

Stadium::Stadium(int columns, int rows,
                 int minGameDurationInMicro, int maxGameDurationInMicro) {
    this->columns = columns;
    this->rows = rows;
    this->fields = vector<Field>(static_cast<unsigned long>(rows * columns));

    Semaforo *entrance = ResourceHandler::getInstance()->createSemaforo(SEM_FILE_FIELD_ENTRANCE, 0, columns * rows);
    Semaforo *exit = ResourceHandler::getInstance()->createSemaforo(SEM_FILE_FIELD_EXIT, 0, columns * rows);
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++) {
            stringstream name;
            name << "[" << i << ", " << j << "}";
            auto id = static_cast<unsigned short>(i * rows + j);
            fields[i * rows + j] = Field(name.str(), entrance, id, exit, id,
                                         minGameDurationInMicro, maxGameDurationInMicro);
        }
    }
}

Field Stadium::getField(int column, int row) {
    return fields[column * rows + row];
}

int Stadium::getColumns() {
    return columns;
}

int Stadium::getRows() {
    return rows;
}
