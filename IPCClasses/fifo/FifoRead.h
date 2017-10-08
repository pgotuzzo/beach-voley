#ifndef BEACH_VOLEY_FIFOREAD_H
#define BEACH_VOLEY_FIFOREAD_H


#include <unistd.h>
#include "Fifo.h"

using namespace std;

class FifoRead : public Fifo {
public:
    FifoRead();

    FifoRead(const string name);

    ~FifoRead();

    int openFifo();

    ssize_t readFifo(void *buffer, const ssize_t buffsize) const;

};


#endif //BEACH_VOLEY_FIFOREAD_H
