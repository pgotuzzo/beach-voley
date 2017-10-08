#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "Player.h"
#include "../InitException.h"
#include "PartnerRequester.h"
#include "../../Logger/Logger.h"
#include "../Constants.h"
#include "../../IPCClasses/FifoWrite.h"

using namespace std;

Player::Player(const string &name, Field *field) {
    this->name = name;
    this->field = field;
    fifo = new FifoWrite(FIFO_FILE_PARTNER_REQUEST);
    int fd = fifo->openFifo();
    if (fd < 0) {
        throw InitException("Partner request fifo can't be opened!");
    }

}

void Player::play() {
    log("Buscando compañero...");
    partnerRequest();
    if (response.playerAction == ENUM_PLAY) {
        log("Compañero asignado!");
        goToPlayCourt();
        leaveCourt();
        play();
    } else {
        log("algo salio mal");
    }
}

void Player::partnerRequest() {
    PartnerRequester::request(name,fifo, getpid());
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
}

SemaforoInfo Player::getSemaforoInfoEntry() {
    return getCourt().getEntry();
}


SemaforoInfo Player::getSemaforoInfoExit() {
    return getCourt().getExit();
}

Court Player::getCourt() {
    return this->field->getCourt(response.row, response.column);
}

void Player::removeTmpFile(string fileName) {

    if (remove(fileName.c_str()) != 0) {
        throw InitException("No se pudo eliminar el archivo temporal " + fileName);
    }

}

void Player::log(string message) {
    string aux = "Player " + name + ": " + message;
    Logger::getInstance()->loguear(aux.c_str());
}

