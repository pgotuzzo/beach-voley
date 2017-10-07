
#include "Court.h"
#include "../../Logger/Logger.h"

Court::Court(string name, Semaforo *entrance, unsigned short entranceId, Semaforo *exit, unsigned short exitId) {
    this->name = name;
    this->entrance = {entranceId, entrance};
    this->exit = {exitId, exit};
}

Court::Court() = default;

void Court::waitForPlayers() {
    log("Esperando por los participantes...");
    for (int i = 0; i < 4; i++) {
        entrance.s->p(entrance.id);
        log("Llego un nuevo participante");
    }
    play();
}

void Court::play() {
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

SemaforoInfo Court::getEntry() {
    return entrance;
}

SemaforoInfo Court::getExit() {
    return exit;
}

void Court::log(string message) {
    string aux = "Cancha " + name + ": " + message;
    Logger::getInstance()->loguear(aux.c_str());
}

