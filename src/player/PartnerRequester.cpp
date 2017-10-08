#include <iostream>
#include "PartnerRequester.h"
#include "../InitException.h"
#include "../Constants.h"
#include "../../IPCClasses/FifoWrite.h"
#include "../../IPCClasses/FifoRead.h"
#include "../../IPCClasses/LockFile.h"

ssize_t PartnerRequester::request(const string &name, FifoWrite *fifo,  pid_t pidt) {
    LockFile* lockFile = new LockFile(LOCK_FILE_PARTNER_REQUEST);
    int res = lockFile->tomarLock();
    if (res < 0) {
        throw InitException("Partner request lock can't be opened!");
    }


    TaskRequest task = {pidt, 0, 0, false, FIND_PARTNER};

    ssize_t out = fifo->writeFifo( static_cast<const void*>(&task), sizeof(TaskRequest));

    if(out < 0) {
        throw InitException("Partner request lock can't be write!");
    }

    cout << "PARTNER_REQUESTER Participante " + name + ": pidió un compañero" << endl;

    lockFile->liberarLock();

    return out;

}

OrgPlayerResponse PartnerRequester::waitResponse(string name) {
    FifoRead* partnerFifo = new FifoRead(name);
    int fd = partnerFifo->openFifo();
    if (fd < 0) {
        throw InitException("Partner response fifo can't be opened!");
    }

    OrgPlayerResponse buffer = {};

    ssize_t out = partnerFifo->readFifo((&buffer), sizeof(OrgPlayerResponse));

    if(out < 0) {
        throw InitException("Partner response fifo can't be write!");
    }

    partnerFifo->closeFifo();

    cout << "PARTNER_REQUESTER Participante " + name + ": recibió un compañero " << buffer.show() << endl;

    return buffer;

}
