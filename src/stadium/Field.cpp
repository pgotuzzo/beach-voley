#include <unistd.h>
#include <cstring>
#include <utility>
#include "Field.h"
#include "../../util/RandomNumber.h"
#include "../../util/ResourceHandler.h"
#include "../config/Constants.h"

#include "../../IPCClasses/signal/SignalHandler.h"
#include "../../IPCClasses/signal/SIGINT_HandlerForField.h"


Field::Field(unsigned short id, string name, Semaforo *entrance, Semaforo *exit, const int minGameDurationInMicro,
             const int maxGameDurationInMicro) :
        name(std::move(name)),
        id(id),
        minGameDurationInMicro(minGameDurationInMicro),
        maxGameDurationInMicro(maxGameDurationInMicro) {
    this->entrance = {id, entrance};
    this->exit = {id, exit};
    this->taskToManagerFifo = ResourceHandler::getInstance()->getFifoWrite(FIFO_FILE_MANAGER_RECEIVE_TASK);
    int fd = taskToManagerFifo->openFifo();
    if (fd < 0) {
        stringstream message;
        message << "Field" << "Trying to open a fifo to write a response. Fifo couldn't be opened. Error Number: "
                << strerror(errno) << " " << errno << endl;
        throw runtime_error(message.str());
    }

    SIGINT_HandlerForField sigint_handlerForField(this);
    SignalHandler :: getInstance()->registrarHandler ( SIGINT,&sigint_handlerForField );
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
 * If there are players to release from the field it release them.
 */
void Field::releasePlayers() {
    if (thereArePlayersToRelease) {
        log("Liberando los participantes...");
        for (int i = 0; i < 4; i++) {
            exit.s->v(exit.id);
        }
    }
    thereArePlayersToRelease = false;
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
    int fd = taskToManagerFifo->openFifo();
    if (fd < 0) {
        stringstream message;
        message << "Field" << "Trying to open a fifo to write a response. Fifo couldn't be opened. Error Number: "
                << strerror(errno) << " " << errno << endl;
        throw runtime_error(message.str());
    }

    TaskRequest taskRequest{id, 3, 3, false, MATCH_RESULT};
    this->setResult(&taskRequest);
    log("Trying to write a response");
    ssize_t out = taskToManagerFifo->writeFifo((&taskRequest), sizeof(TaskRequest));
    if (out < 0) {
        throw runtime_error(string("Match return fifo can't be write!") + strerror(errno));
    }
    taskToManagerFifo->closeFifo();
}

/**
 * The field waits till the player arrive. Waits until the game ends.
 * Gets a random result from the match. Leaves the players free, and start again
 * until the tournament ends.
 */
void Field::readyForGames() {
    bool tournamentEnded = false;
    // TODO: This must be checked in a shared memory
    while (!tournamentEnded) {
        waitForPlayers();
        thereArePlayersToRelease = true;
        if (!flooded) {
            log("Comenzo el partido");
            usleep(getRandomUnsignedInt(minGameDurationInMicro, maxGameDurationInMicro));
            if (!flooded) {
                log("Se comunica el resultado");
                sendResult();
                log("Finalizo el partido");
            } else {
                log("El partido se cancelo por inundacion");
            }
        }
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

/**
 * Toggles the flooded bool and sends a notification to the manager about the tide change.
 */
void Field::toggleFloodedAndSendNotification() {
    flooded = !flooded;
    // if now is flooded then the tideRise
    TaskRequest taskRequest{id, 0, 0, flooded, TIDE_CHANGE};
    this->setResult(&taskRequest);
    log("Trying to write a response");
    ssize_t out = taskToManagerFifo->writeFifo((&taskRequest), sizeof(TaskRequest));
    if (out < 0) {
        throw runtime_error(string("Match return fifo can't be write!") + strerror(errno));
    }
}

Field::~Field() {
    SignalHandler::destruir();
    taskToManagerFifo->closeFifo();
}
