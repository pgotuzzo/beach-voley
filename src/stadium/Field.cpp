
#include <unistd.h>
#include "Field.h"
#include "../../util/RandomNumber.hpp"

Field::Field(string name, Semaforo *entrance, unsigned short entranceId, Semaforo *exit, unsigned short exitId,
             const int minGameDurationInMicro, const int maxGameDurationInMicro) :
        minGameDurationInMicro(minGameDurationInMicro), maxGameDurationInMicro(maxGameDurationInMicro) {
    this->name = name;
    this->entrance = {entranceId, entrance};
    this->exit = {exitId, exit};
}

void Field::waitForPlayers() {
    log("Esperando por los participantes...");
    for (int i = 0; i < 4; i++) {
        entrance.s->p(entrance.id);
        log("Llego un nuevo participante");
    }
    play();
}

MatchResult Field::getResult() {
    unsigned int teamThatLose = getRandomUnsignedInt(0, 2);
    int teams[2] = {3, 3};
    teams[teamThatLose] = getRandomInt(0, 3);

    return MatchResult{teams[0], teams[1]};
}

/**
 * The field waits till the game ends. Gets the random result.
 * Leaves the players free
 */
void Field::play() {
    log("Comenzo el partido");
    usleep(getRandomUnsignedInt(minGameDurationInMicro, maxGameDurationInMicro));
    MatchResult result = getResult();
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

