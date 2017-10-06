//
// Created by sebastian on 03/10/17.
//

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include "PartnerRequester.h"
#include "../InitException.h"
#include "../Constants.h"
#include "FifoWrite.h"

ssize_t PartnerRequester::request(const string &name, pid_t pid) {
    FifoWrite* partnerFifo = new FifoWrite(FIFO_FILE_PARTNER_REQUEST);
    int fd = partnerFifo->openFifo();
    if (fd < 0) {
        throw InitException("Partner request fifo can't be opened!");
    }

    ssize_t out = partnerFifo->writeFifo(reinterpret_cast<const void *>(pid), sizeof(pid));

    cout << "Participante " + name + ": pidió un compañero" << endl;

    return out;

}
