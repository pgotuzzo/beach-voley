#ifndef BEACH_VOLEY_FIFOWRITE_H
#define BEACH_VOLEY_FIFOWRITE_H


#include "Fifo.h"

class FifoWrite : public Fifo {
public:
    FifoWrite();

    FifoWrite(const std::string nombre);

    ~FifoWrite();

    int openFifo();

    ssize_t writeFifo(const void *buffer, ssize_t buffsize) const;
};


#endif //BEACH_VOLEY_FIFOWRITE_H
