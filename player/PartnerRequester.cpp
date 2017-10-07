//
// Created by sebastian on 03/10/17.
//

#include <iostream>
#include "PartnerRequester.h"
#include "../InitException.h"
#include "../Constants.h"
#include "FifoWrite.h"
#include "FifoRead.h"

ssize_t PartnerRequester::request(const string &name, pid_t pid) {
    FifoWrite* partnerFifo = new FifoWrite(FIFO_FILE_PARTNER_REQUEST);
    int fd = partnerFifo->openFifo();
    if (fd < 0) {
        throw InitException("Partner request fifo can't be opened!");
    }

    ssize_t out = partnerFifo->writeFifo( static_cast<const void*>(to_string(pid).c_str()), sizeof(to_string(pid)));

    if(out < 0) {
        throw InitException("Partner request fifo can't be write!");
    }

    cout << "Participante " + name + ": pidió un compañero" << endl;

    partnerFifo->closeFifo();

    return out;

}

void PartnerRequester::waitResponse(string name) {
    FifoRead* partnerFifo = new FifoRead(name);
    int fd = partnerFifo->openFifo();
    if (fd < 0) {
        throw InitException("Partner response fifo can't be opened!");
    }

    char buffer[BUFFSIZE];

    ssize_t out = partnerFifo->readFifo((buffer), BUFFSIZE);

    if(out < 0) {
        throw InitException("Partner request fifo can't be write!");
    }

    std::string mensaje = buffer;
    mensaje.resize ( out );

    partnerFifo->closeFifo();

    cout << "Participante " + name + ": recibió un compañero " << mensaje << endl;

}
