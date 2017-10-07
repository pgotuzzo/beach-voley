#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "Player.h"
#include "../InitException.h"
#include "PartnerRequester.h"
#include "../../Logger/Logger.h"
#include "../Constants.h"

using namespace std;

Player::Player(const string &name, Field *field, const Semaforo *fieldTurnstile): fieldTurnstile(fieldTurnstile) {
    this->name = name;
    this->field = field;

    this->logger = Logger::getInstance();
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
                response->playerAction !=ENUM_LEAVE_STADIUM) {
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
    PartnerRequester::request(name, getpid());
    organizatorResponse();
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

void Player::organizatorResponse() {
    string file = FIFO_FILE_PARTNER_RESPONSE + to_string(getpid());
    response = PartnerRequester::waitResponse(file);
    removeTmpFile(file);
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

void Player::removeTmpFile(string fileName) {

    if (remove(fileName.c_str()) != 0) {
        throw InitException("No se pudo eliminar el archivo temporal " + fileName);
    }

}

void Player::log(string message) {
    string aux = "Player " + name + ": " + message;
    this->logger->logMessage(aux.c_str());
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

