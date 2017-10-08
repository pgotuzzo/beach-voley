//
// Created by sebastian on 07/10/17.
//

#include "Manager.h"
#include "../../IPCClasses/FifoRead.h"
#include "../Constants.h"
#include "../InitException.h"
#include "../Definitions.h"
#include "../../IPCClasses/FifoWrite.h"
#include "../../IPCClasses/LockFile.h"

void Manager::receiveTask() {

    FifoRead *fifo = new FifoRead(FIFO_FILE_PARTNER_REQUEST);

    cout<< "MANAGER crea fifo"<<endl;
    int fd = fifo->openFifo();
    cout<< "MANAGER abrió fifo"<<endl;
    if (fd < 0) {
        throw InitException("Partner request fifo can't be opened!");
    }


    TaskRequest task = {};
    cout<< "MANAGER va a leer fifo"<<endl;
    ssize_t out = fifo->readFifo((&task), sizeof(TaskRequest));

    cout<< "MANAGER recibe tarea"<<endl;

    fifo->closeFifo();

    if (out < 0) {
        throw InitException("Partner request fifo can't be write!");
    }

    if (out < sizeof(TaskRequest)) {
        throw InitException("Partner request fifo can't be write!");
    }


    switch (task.task) {
        case (FIND_PARTNER): findPartner(task.pid);
            break;
        case (MATCH_CANCELLED):
        case (MATCH_RESULT):
        default: throw InitException("Task handler not defined.");
    }

    receiveTask();

}

void Manager::findPartner(int pid) {

    //TODO: find partner + find court

    FifoWrite *partnerFifo = new FifoWrite(FIFO_FILE_PARTNER_RESPONSE + to_string(pid));
    int fd = partnerFifo->openFifo();

    if (fd < 0) {
        throw InitException("Partner response fifo can't be opened!");
    }

    OrgPlayerResponse response = {0, 0, ENUM_PLAY};

    ssize_t out = partnerFifo->writeFifo(static_cast<const void *> (&response), sizeof(OrgPlayerResponse));

    if (out < 0) {
        throw InitException("Partner response fifo can't be write!");
    }

    cout << "MANAGER Se consiguió un compañero" <<  to_string(pid) << endl;


}

Manager::Manager() = default;


