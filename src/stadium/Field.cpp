#include <unistd.h>
#include <cstring>
#include "Field.h"
#include "../../util/RandomNumber.h"
#include "../../util/ResourceHandler.h"
#include "../config/Constants.h"

Field::Field(string name, Semaforo *entrance, unsigned short entranceId, Semaforo *exit, unsigned short exitId,
             const int minGameDurationInMicro, const int maxGameDurationInMicro) :
        minGameDurationInMicro(minGameDurationInMicro), maxGameDurationInMicro(maxGameDurationInMicro) {
    this->name = name;
    this->entrance = {entranceId, entrance};
    this->exit = {exitId, exit};
    this->taskToManagerFifo = ResourceHandler::getInstance()->createFifoWirte(FIFO_FILE_MANAGER_RECEIVE_TASK);
    int fd = taskToManagerFifo->openFifo();
    if (fd < 0) {
        stringstream message;
        message << "Field" << "Trying to open a fifo to write a response. Fifo couldn't be opened. Error Number: " << strerror(errno) << " " <<errno << endl;
        throw runtime_error(message.str());
    }
}

/**
 * Waits for the players to be in the field.
 */
void Field::waitForPlayers() {
    log("Esperando por los participantes...");
    for (int i = 0; i < 4; i++) {
        entrance.s->p(entrance.id);
        log("Llego un nuevo participante");
    }
}

/**
 * Release the players from the field.
 */
void Field::releasePlayers() {
    log("Liberando los participantes...");
    for (int i = 0; i < 4; i++) {
        exit.s->v(exit.id);
    }
}

/**
 * Creates a random result for a match and set it in the task request.
 * One of the team will win 3 sets, and the other a random number
 * between 0 and 2.
 */
void Field::setResult(TaskRequest *taskRequest) {
    unsigned int teamThatLose = getRandomUnsignedInt(0, 2);
    int teams[2] = {3, 3};
    teams[teamThatLose] = getRandomInt(0, 3);

    taskRequest->resultLocal = teams[0];
    taskRequest->resultLocal = teams[1];
}

/**
 * Sends the result of the match to the manager.
 */
void Field::sendResult() {
    int pid = getpid();
    TaskRequest taskRequest{pid, 3, 3, false, MATCH_RESULT};
    this->setResult(&taskRequest);
    log("Trying to write a response");
    ssize_t out = taskToManagerFifo->writeFifo((&taskRequest), sizeof(TaskRequest));
    if (out < 0) {
        throw runtime_error(string("Match return fifo can't be write!" ) + strerror(errno));
    }
}

/**
 * The field waits till the player arrive. Waits until the game ends.
 * Gets a random result from the match. Leaves the players free, and start again
 * until the tournament ends.
 */
void Field::readyForGames() {
    bool tournamentEnded = false;
    // TODO: This must be checked in a shared memory
    while(!tournamentEnded) {
        waitForPlayers();
        log("Comenzo el partido");
        usleep(getRandomUnsignedInt(minGameDurationInMicro, maxGameDurationInMicro));
        log("Se comunica el resultado");
        sendResult();
        log("Finalizo el partido");
        releasePlayers();
    }
}

SemaforoInfo Field::getEntry() {
    return entrance;
}

SemaforoInfo Field::getExit() {
    return exit;
}

/**
 * Logs a message with the aditional field info.
 *
 * @param message the message to log.
 */
void Field::log(string message) {
    string aux = "Cancha " + name + ": " + message;
    Logger::getInstance()->logMessage(aux.c_str());
}

