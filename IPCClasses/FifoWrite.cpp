//
// Created by sebastian on 03/10/17.
//

#include "FifoWrite.h"

FifoWrite::FifoWrite(const std::string nombre) : Fifo(nombre) {
}

FifoWrite::~FifoWrite() {
}

int  FifoWrite::openFifo() {
    fd = open ( name.c_str(),O_WRONLY );

    return fd;
}

ssize_t FifoWrite::writeFifo(const void* buffer, ssize_t buffsize) const {
    return write ( fd,buffer,buffsize );
}