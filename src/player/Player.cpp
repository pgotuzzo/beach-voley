#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "Player.h"
#include "../InitException.h"
#include "PartnerRequester.h"

using namespace std;

Player::Player(const string &name, Field *field) {
    this->name = name;
    this->field = field;
}

void Player::subscribe() {
    int fd = open(FIFO_FILE_TOURNAMENT, O_WRONLY, 0644);
    if (fd < 0) {
        throw InitException("Tournament fifo can't be opened!");
    }
    cout << "Participante " + name + ": pudo conectarse con Tournament" << endl;
}

void Player::play() {
    partnerRequest();
    goToPlayCourt();
    leaveCourt();
}

void Player::partnerRequest() {
    PartnerRequester::request(name, getpid());
    organizatorResponse();
}

void Player::goToPlayCourt() {
    SemaforoInfo semInfo = getSemaforoInfoEntry();
    semInfo.s->v(semInfo.id);
}

void Player::leaveCourt() {
    SemaforoInfo semInfo = getSemaforoInfoExit();
    semInfo.s->p(semInfo.id);
}

void Player::organizatorResponse() {
    string fileName = FIFO_FILE_PARTNER_RESPONSE + to_string(getpid());
    string path = "/tmp/" + fileName;
    response = PartnerRequester::waitResponse(path);
    removeTmpFile(path);
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

