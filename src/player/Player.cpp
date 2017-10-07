#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "Player.h"
#include "../InitException.h"
#include "PartnerRequester.h"

using namespace std;

Player::Player(const string &name) {
    this->name = name;
}

void Player::subscribe() {
    int fd = open(FIFO_FILE_TOURNAMENT, O_WRONLY, 0644);
    if (fd < 0) {
        throw InitException("Tournament fifo can't be opened!");
    }
    cout << "Participante " + name + ": pudo conectarse con Tournament" << endl;
}

void Player::partnerRequest() {
    PartnerRequester::request(name, getpid());
}

void Player::organizatorResponse() {
    string fileName =  FIFO_FILE_PARTNER_RESPONSE + getpid();
    string path = "/tmp/" + fileName;
    PartnerRequester::waitResponse(path);
    removeTmpFile(path);
}

void Player::removeTmpFile(string fileName) {

    if( remove( fileName.c_str() ) != 0 ) {
        throw InitException("No se pudo eliminar el archivo temporal " + fileName);
    }

}

