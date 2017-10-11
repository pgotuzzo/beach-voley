#include <sstream>
#include "Stadium.h"
#include "../config/Constants.h"
#include "../../util/ResourceHandler.h"

Stadium::Stadium(int columns, int rows,
                 int minGameDurationInMicro, int maxGameDurationInMicro) {
    this->columns = columns;
    this->rows = rows;
    this->fields = vector<Field>(static_cast<unsigned long>(rows * columns));

    Semaforo *entrance = ResourceHandler::getInstance()->getSemaforo(SEM_FILE_FIELD_ENTRANCE);
    Semaforo *exit = ResourceHandler::getInstance()->getSemaforo(SEM_FILE_FIELD_EXIT);
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++) {
            stringstream name;
            name << "[" << i << ", " << j << "]";
            auto id = static_cast<unsigned short>(i * rows + j);
            fields[i * rows + j] =
                    Field(id, name.str(), entrance, exit, minGameDurationInMicro, maxGameDurationInMicro);
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

/**
 * Start all the field of the stadium, so the games can start.
 * Every field is a different process, that will end when
 * the tournament ends.
 */
void Stadium::initStadium() {
    for (int i = 0; i < this->getColumns(); i++) {
        for (int j = 0; j < this->getRows(); j++) {
            Field field = this->getField(i, j);
            int pid = fork();
            if (pid == 0) {
                field.readyForGames();
                exit(0);
            }
        }
    }
}
