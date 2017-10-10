
#include <iostream>
#include "Player.h"

using namespace std;

Player::Player(const string &name, Stadium *stadium, const Semaforo *stadiumTurnstile) : stadiumTurnstile(
        stadiumTurnstile) {
    this->name = name;
    this->stadium = stadium;
    this->requester = new PartnerRequester(name);
}

void Player::init() {
    this->requester->init();
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
//        enterStadium();
        log("Buscando compañero...");
        partnerRequest();
        while (response.playerAction != ENUM_LEAVE_TOURNAMENT or
               response.playerAction != ENUM_LEAVE_STADIUM) {
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

void Player::partnerRequest() {
    requester->request();
    response = requester->waitResponse();
}

void Player::goToPlayGame() {
    SemaforoInfo semInfo = getSemaforoInfoEntry();
    string aux = "Yendo a la cancha: " + to_string(semInfo.id);
    log(aux);
    semInfo.s->v(semInfo.id);
    log("Adentro de la cancha");
}

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
    return this->stadium->getField(response.row, response.column);
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

