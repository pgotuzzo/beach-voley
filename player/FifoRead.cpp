//
// Created by sebastian on 03/10/17.
//

#include "FifoRead.h"

FifoRead::FifoRead(const std::string name) : Fifo(name) {
}

FifoRead::~FifoRead() {
}

int FifoRead::openFifo() {
    fd = open ( name.c_str(),O_RDONLY );

    return fd;
}

ssize_t FifoRead::readFifo(void *buffer, const ssize_t buffsize) const {
    return read(fd, buffer, buffsize);
}