#include "Player.h"
#include "../logger/Logger.h"
#include <cstring>

using namespace std;

/**
 * Logs a message with aditional information about the process and class.
 * @param message the message to log
 */
void Player::logMessage(const string &message) {
    string messageToLog = to_string(getpid()) + string(" Player ") + to_string(id) +string(": ") + message;
    Logger::getInstance()->logMessage(messageToLog.c_str());
}

/**
 * Player constructor initialize the attributes.
 *
 * @param id the player identifier
 * @param entranceToFields the semaphore that represent the entrance to the fields where players will announce that
 *          they arrive to the field
 * @param exitFromFields the semaphore that represent the exit of the field where players will wait till the game ends
 * @param stadiumTurnstile the semaphore that represents the entrance to the stadium
 * @param receiveResponsesPipe the pipe to receive the responses from the manager
 * @param sendRequestPipe the pipe to send partner request to the manager
 */
Player::Player(int id, const Semaforo *entranceToFields, const Semaforo *exitFromFields,
               const Semaforo *stadiumTurnstile, Pipe *receiveResponsesPipe, Pipe *sendRequestPipe):
        id(id), entranceToFields(entranceToFields), exitFromFields(exitFromFields),
        stadiumTurnstile(stadiumTurnstile), receiveResponsesPipe(receiveResponsesPipe),
        sendRequestPipe(sendRequestPipe) {
    sendRequestPipe->setearModo(Pipe::ESCRITURA);
    receiveResponsesPipe->setearModo(Pipe::LECTURA);
}

/**
 * The player starts playing.
 * The player will try to enter the stadium as long as the tournament doesn't finish.
 * Once inside the stadium he will search for a partner, go play the games, and search
 * for new partners again, till he couldn't have partner or the tournament ends.
 */
void Player::play() {
    bool leaveTournament = false;
    while (!leaveTournament) {
        enterStadium();
        OrgPlayerResponse response = partnerRequest();
        while (response.playerAction == ENUM_PLAY) {
            goToPlayGame(response.fieldId);
            response = partnerRequest();
        }
        if (response.playerAction == ENUM_LEAVE_TOURNAMENT) {
            leaveTournament = true;
        }
        leaveStadium();
    }
    logMessage("Leaving the tournament");
}

/**
 * Player make the request to the Manager over pipe where manager receives tasks
 * and waits for the response from Manager in the other pipe.
 *
 */
OrgPlayerResponse Player::partnerRequest() {
    logMessage("Searching partner");
    TaskRequest taskRequest = {id, 0, 0, false, FIND_PARTNER};
    ssize_t out = this->sendRequestPipe->escribir(&taskRequest, sizeof(TaskRequest));
    if (out < 0) {
        std::cout << strerror(errno)<< std::endl;
        throw runtime_error("Partner request pipe can't be write!");
    }

    OrgPlayerResponse response{};
    out = receiveResponsesPipe->leer((&response), sizeof(OrgPlayerResponse));
    if (out < 0) {
        throw runtime_error("PartnerRequester: response couldn't be read! Error Number: " + errno);
    }

    return response;
}

/**
 * Enter in to the Field and plays the game.
 */
void Player::goToPlayGame(unsigned short fieldId) {
    logMessage("Entering the field " + to_string(fieldId));
    entranceToFields->v(fieldId);
    exitFromFields->p(fieldId);
    logMessage("Leaving the field " + to_string(fieldId));
}

/**
 * It waits till there is place inside the stadium.
 */
void Player::enterStadium() {
    logMessage("Waiting to enter the stadium");
    stadiumTurnstile->p(0);
}

/**
 * It leaves the stadium
 */
void Player::leaveStadium() {
    logMessage("Leaving the stadium");
    stadiumTurnstile->v(0);
}

