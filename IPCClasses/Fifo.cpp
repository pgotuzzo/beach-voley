//
// Created by sebastian on 03/10/17.
//

#include "Fifo.h"

Fifo::Fifo(const std::string name) : name(name), fd(-1) {
    mknod ( static_cast<const char*>(name.c_str()),S_IFIFO|0666,0 );
}

Fifo::~Fifo() {
}

void Fifo::closeFifo() {
    close ( fd );
    fd = -1;
}

void Fifo::deleteFifo() const {
    unlink ( name.c_str() );
}