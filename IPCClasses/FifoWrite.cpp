#include "FifoWrite.h"

FifoWrite::FifoWrite(const std::string nombre) : Fifo(nombre) {
}

FifoWrite::~FifoWrite() {
}

int FifoWrite::openFifo() {
    fd = open(name.c_str(), O_WRONLY | O_CREAT);

    return fd;
}

ssize_t FifoWrite::writeFifo(const void *buffer, ssize_t buffsize) const {
    return write(fd, buffer, buffsize);
}