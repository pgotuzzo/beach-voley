//
// Created by sebastian on 03/10/17.
//

#ifndef BEACH_VOLEY_FIFOREAD_H
#define BEACH_VOLEY_FIFOREAD_H


#include <unistd.h>
#include "Fifo.h"

class FifoRead : public Fifo {
public:
    FifoRead(const std::string name);
    ~FifoRead();

    int openFifo();
    ssize_t readFifo(void *buffer, const ssize_t buffsize) const;

};


#endif //BEACH_VOLEY_FIFOREAD_H
