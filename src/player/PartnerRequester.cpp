#include <iostream>
#include "PartnerRequester.h"
#include "../config/Constants.h"
#include "../../util/ResourceHandler.h"


PartnerRequester::PartnerRequester(const string &playerName) {
    this->playerName = playerName;
    this->fifoRead = ResourceHandler::getInstance()->createFifoRead(FIFO_FILE_PARTNER_RESPONSE + to_string(getpid()));
    this->fifoWrite = ResourceHandler::getInstance()->createFifoWirte(FIFO_FILE_PARTNER_REQUEST);
}

void PartnerRequester::request() {
    int fd = fifoWrite->openFifo();
    if (fd < 0) {
        throw runtime_error(
                "PartnerRequester: Trying to open a fifo to write a request to find a partner. Fifo couldn't be opened. Error Number: " +
                errno);
    }
    int pid = getpid();
    ssize_t out = fifoWrite->writeFifo(static_cast<const void *>(&pid), sizeof(int));
    if (out < 0) {
        throw runtime_error("Partner request fifo can't be write!");
    }
    cout << "Player " << playerName << ": sent a find partner request" << endl;
}

OrgPlayerResponse *PartnerRequester::waitResponse() {
    int fd = fifoRead->openFifo();
    if (fd < 0) {
        throw runtime_error(
                "PartnerRequester: Trying to open fifo to read a response. Fifo couldn't be opened. Error Number: " +
                errno);
    }
    auto *buffer = new OrgPlayerResponse;
    ssize_t out = fifoRead->readFifo((buffer), sizeof(buffer));

    if (out < 0) {
        throw runtime_error("PartnerRequester: response couldn't be read! Error Number: " + errno);
    }

    cout << "Player " << playerName << ": received a response: " << buffer->show() << endl;

    return buffer;
}
