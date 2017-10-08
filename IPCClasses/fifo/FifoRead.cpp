#include "FifoRead.h"

FifoRead::FifoRead() = default;

FifoRead::FifoRead(const std::string name) : Fifo(name) {
}

FifoRead::~FifoRead() {
}

int FifoRead::openFifo() {
    fd = open(name.c_str(), O_RDONLY | O_CREAT);

    return fd;
}

ssize_t FifoRead::readFifo(void *buffer, const ssize_t buffsize) const {
    return read(fd, buffer, buffsize);
}
