#ifndef BEACH_VOLEY_MANAGER_H
#define BEACH_VOLEY_MANAGER_H

#include <map>
#include "../../IPCClasses/fifo/FifoRead.h"
#include "../../IPCClasses/fifo/FifoWrite.h"
#include "../player/Player.h"

using namespace std;

class Manager {
private:
    map<int, FifoWrite> mFifoWrite;
    FifoRead *fifoRead;

    void findPartner(int pid);

public:
    Manager();

    void receiveTask();

};


#endif //BEACH_VOLEY_MANAGER_H