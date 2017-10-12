#include <iostream>
#include <cstring>
#include "PartnerRequester.h"


PartnerRequester::PartnerRequester(int playerId, const string &playerName, Pipe *receiveResponsesPipe,
                                   Pipe *sendRequestPipe) :
        playerId(playerId), playerName(playerName), receiveResponsesPipe(receiveResponsesPipe),
        sendRequestPipe(sendRequestPipe) {
}

void PartnerRequester::request() {
    TaskRequest taskRequest = {playerId, 0, 0, false, FIND_PARTNER};
    this->sendRequestPipe->setearModo(Pipe::ESCRITURA);
    ssize_t out = this->sendRequestPipe->escribir(&taskRequest, sizeof(TaskRequest));
    if (out < 0) {
        std::cout << strerror(errno)<< std::endl;
        throw runtime_error("Partner request pipe can't be write!");
    }
    cout << "Player " << playerName << ": sent a find partner request" << endl;
}

OrgPlayerResponse PartnerRequester::waitResponse() {
    OrgPlayerResponse buffer = {};
    ssize_t out = receiveResponsesPipe->leer((&buffer), sizeof(OrgPlayerResponse));

    if (out < 0) {
        throw runtime_error("PartnerRequester: response couldn't be read! Error Number: " + errno);
    }

    cout << "Player " << playerName << ": received a response: " << endl << buffer.show() << endl;

    return buffer;
}
