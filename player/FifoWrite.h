//
// Created by sebastian on 03/10/17.
//

#ifndef BEACH_VOLEY_FIFOWRITE_H
#define BEACH_VOLEY_FIFOWRITE_H


#include "Fifo.h"

class FifoWrite : public Fifo{
public:
    FifoWrite(const std::string nombre);
    ~FifoWrite();

    int openFifo();
    ssize_t writeFifo(const void* buffer,const ssize_t buffsize) const;
};


#endif //BEACH_VOLEY_FIFOWRITE_H
