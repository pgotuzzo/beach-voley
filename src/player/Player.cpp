
#include <iostream>
#include "Player.h"

using namespace std;

Player::Player(int id, const string &name, Stadium *stadium, const Semaforo *stadiumTurnstile) :
        id(id),
        name(name),
        stadium(stadium),
        stadiumTurnstile(stadiumTurnstile) {
    this->requester = new PartnerRequester(id, name);
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
        log("Entrando al predio...");
        enterStadium();
        log("Buscando compañero...");
        partnerRequest();
        while (response.playerAction == ENUM_PLAY) {
            log("Compañero asignado! Yendo a jugar");
            goToPlayGame();
            log("Dejando la cancha");
            leaveField();
            log("Buscando compañero...");
            partnerRequest();
        }
        if (response.playerAction == ENUM_LEAVE_TOURNAMENT) {
            leaveTournament = true;
        }
        log("Saliendo del predio...");
        leaveStadium();
    }
    log("Saliendo del torneo...");
}
/**
 * Player make the request to the Manager over fifo FIFO_FILE_MANAGER_RECEIVE_TASK
 * and waits for the response Manager in the other Fifo FIFO_FILE_PARTNER_RESPONSE + id
 * */
void Player::partnerRequest() {
    requester->request();
    response = requester->waitResponse();
}
/**
 * Enter in to the Field and .
 * */
void Player::goToPlayGame() {
    SemaforoInfo semInfo = getSemaforoInfoEntry();
    string aux = "Yendo a la cancha: " + to_string(semInfo.id);
    log(aux);
    semInfo.s->v(semInfo.id);
    log("Adentro de la cancha");
}

/**
 * Leaves the field when the Field release the semaphore
 * */
void Player::leaveField() {
    SemaforoInfo semInfo = getSemaforoInfoExit();
    semInfo.s->p(semInfo.id);
}

SemaforoInfo Player::getSemaforoInfoEntry() {
    return getField().getEntry();
}


SemaforoInfo Player::getSemaforoInfoExit() {
    return getField().getExit();
}

Field Player::getField() {
    return this->stadium->getField(response.fieldId);
}

void Player::log(string message) {
    string aux = "Player " + name + ": " + message;
    Logger::getInstance()->logMessage(aux.c_str());
}

/**
 * It waits till there is place inside the stadium.
 */
void Player::enterStadium() {
    stadiumTurnstile->p(0);
}

/**
 * It leaves the stadium
 */
void Player::leaveStadium() {
    stadiumTurnstile->v(0);
}

