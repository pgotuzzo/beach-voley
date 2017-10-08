
#include <iostream>
#include "Player.h"

using namespace std;

Player::Player(const string &name, Field *field, const Semaforo *fieldTurnstile) : fieldTurnstile(fieldTurnstile) {
    this->name = name;
    this->field = field;
    this->requester = new PartnerRequester(name);
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
        enterField();
        log("Buscando compañero...");
        partnerRequest();
        while (response->playerAction != ENUM_LEAVE_TOURNAMENT or
               response->playerAction != ENUM_LEAVE_STADIUM) {
            log("Compañero asignado! Yendo a jugar");
            goToPlayCourt();
            log("Dejando la cancha");
            leaveCourt();
            log("Buscando compañero...");
            partnerRequest();
        }
        if (response->playerAction == ENUM_LEAVE_TOURNAMENT) {
            leaveTournament = true;
        }
        log("Saliendo del predio...");
        leaveField();
    }
    log("Saliendo del torneo...");
}

void Player::partnerRequest() {
    requester->request();
    response = requester->waitResponse();
}

void Player::goToPlayCourt() {
    SemaforoInfo semInfo = getSemaforoInfoEntry();
    string aux = "Yendo a la cancha: " + to_string(semInfo.id);
    log(aux);
    semInfo.s->v(semInfo.id);
    log("Adentro de la cancha");
}

void Player::leaveCourt() {
    SemaforoInfo semInfo = getSemaforoInfoExit();
    semInfo.s->p(semInfo.id);
}

SemaforoInfo Player::getSemaforoInfoEntry() {
    return getCourt().getEntry();
}


SemaforoInfo Player::getSemaforoInfoExit() {
    return getCourt().getExit();
}

Court Player::getCourt() {
    return this->field->getCourt(response->row, response->column);
}

void Player::log(string message) {
    string aux = "Player " + name + ": " + message;
    Logger::getInstance()->logMessage(aux.c_str());
}

/**
 * It waits till there is place inside the field.
 */
void Player::enterField() {
    fieldTurnstile->p(0);
}

/**
 * It leaves the field
 */
void Player::leaveField() {
    fieldTurnstile->v(0);
}

