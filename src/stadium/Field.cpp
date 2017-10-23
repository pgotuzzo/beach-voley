#include <unistd.h>
#include <cstring>
#include <utility>
#include "Field.h"

#include "../../IPCClasses/signal/SignalHandler.h"
#include "../../IPCClasses/signal/SIGINT_HandlerForField.h"
#include "../util/RandomNumber.h"
#include "../logger/Logger.h"


/**
 * Logs a message with aditional information about the process and class.
 * @param message the message to log
 */
void Field::logMessage(const string &message) {
    string messageToLog = to_string(getpid()) + string(" Field ") + to_string(id) +string(": ") + message;
    Logger::getInstance()->logMessage(messageToLog.c_str());
}

/**
 * Field constructor initialize the attributes. Creates a handler to manage the tides.
 *
 * @param id the field identifier
 * @param entranceSemaphore the semaphore that represent the entrance to the field where players will announce that
 *          they arrive to the field
 * @param exitSempahore the semaphore that represent the exit of the field where players will wait till the game ends
 * @param taskToManagerPipe pipe to send task to the manager
 * @param minGameDurationInMicro minimum quantity of microseconds for the game duration
 * @param maxGameDurationInMicro maximum quantity of microseconds for the game duration
 */
Field::Field(unsigned short id, Semaforo *entranceSemaphore, Semaforo *exitSempahore, Pipe *taskToManagerPipe,
             int minGameDurationInMicro, int maxGameDurationInMicro) :
        id(id), tasksToManagerPipe(taskToManagerPipe), minGameDurationInMicro(minGameDurationInMicro),
        maxGameDurationInMicro(maxGameDurationInMicro), entranceSemaphore(entranceSemaphore), exitSemahore(exitSempahore) {
    taskToManagerPipe->setearModo(Pipe::ESCRITURA);
}

/**
 * Waits for the players to be in the field, and start the game.
 */
void Field::waitForPlayersAndStartGame() {
    logMessage("Waiting for players to arrive");
    for (int i = 0; i < 4; i++) {
        entranceSemaphore->p(id);
        logMessage("A new player arrived");
    }
    inGame = true;
}

/**
 * If there are players playing it release them from the field.
 */
void Field::releasePlayersAndEndGame() {
    if (inGame) {
        logMessage("Releasing the players");
        for (int i = 0; i < 4; i++) {
            exitSemahore->v(id);
        }
        inGame = false;
    }
}

/**
 * Creates a random result for a match and set it in the task request.
 * One of the team will win 3 sets, and the other a random number
 * between 0 and 2.
 */
void setResult(TaskRequest *taskRequest) {
    unsigned int teamThatLose = getRandomUnsignedInt(0, 2);
    int teams[2] = {3, 3};
    teams[teamThatLose] = getRandomInt(0, 3);

    taskRequest->resultLocal = teams[0];
    taskRequest->resultVisitant = teams[1];
}

/**
 * Sends the result of the match to the manager pipe.
 */
void Field::sendResult() {
    TaskRequest taskRequest{id, 3, 3, false, MATCH_RESULT};
    setResult(&taskRequest);
    logMessage("Sending result to manager");
    ssize_t out = tasksToManagerPipe->escribir(&taskRequest, sizeof(TaskRequest));
    if (out < 0) {
        throw runtime_error(string("Match return pipe can't be write! in sendResult for id: ") + to_string(id) +
                                    string(" Error: ") + strerror(errno));
    }
}

/**
 * The field waits till the player arrive. Waits until the game ends.
 * Gets a random result from the match. Leaves the players free, and start again
 * until the tournament ends and sends a SIGTERM.
 */
void Field::startPlayingGames() {
    SIGINT_HandlerForField sigint_handlerForField(this);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handlerForField);
    while (true) {
        waitForPlayersAndStartGame();
        if (!flooded) {
            logMessage("The players are in the field and the game began");
            usleep(getRandomUnsignedInt(minGameDurationInMicro, maxGameDurationInMicro));
            if (!flooded) {
                sendResult();
                releasePlayersAndEndGame();
            }
        } else {
            sleep(2);
            logMessage("Cant play the game because the field is flooded");
            releasePlayersAndEndGame();
        }
    }
}

/**
 * Toggles the flooded bool and sends a notification to the manager about the tide change.
 */
void Field::toggleFloodedAndSendNotification() {
    flooded = !flooded;
    logMessage("The field flooded");
    // if now is flooded then the tide rises
    TaskRequest taskRequest{id, 0, 0, flooded, TIDE_CHANGE};
    logMessage("Sending flood notification to manager");
    ssize_t out = tasksToManagerPipe->escribir((&taskRequest), sizeof(TaskRequest));
    if (out < 0) {
        throw runtime_error(string("Match return fifo can't be write! in toggleFloodedAndSendNotification for id: ")
                            + to_string(id) + string(" Error: ") + strerror(errno));
    }
}