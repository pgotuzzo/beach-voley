
#include "Field.h"

Field::Field(string name, Semaforo *entrance, unsigned short entranceId, Semaforo *exit, unsigned short exitId) {
    this->name = name;
    this->entrance = {entranceId, entrance};
    this->exit = {exitId, exit};
}

Field::Field() = default;

void Field::waitForPlayers() {
    log("Esperando por los participantes...");
    for (int i = 0; i < 4; i++) {
        entrance.s->p(entrance.id);
        log("Llego un nuevo participante");
    }
    play();
}

void Field::play() {
    log("Comenzo el partido");
    // TODO - Create result
    log("Finalizo el partido");
    for (int i = 0; i < 4; i++) {
        exit.s->v(exit.id);
    }
    log("Se comunica el resultado");
    // TODO - Send result
    waitForPlayers();
}

SemaforoInfo Field::getEntry() {
    return entrance;
}

SemaforoInfo Field::getExit() {
    return exit;
}

void Field::log(string message) {
    string aux = "Cancha " + name + ": " + message;
    Logger::getInstance()->logMessage(aux.c_str());
}

