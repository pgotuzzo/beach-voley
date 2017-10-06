#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "Player.h"
#include "../Constants.h"
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
