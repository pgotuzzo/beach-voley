#include "Pipe.h"

Pipe::Pipe() : lectura(true), escritura(true) {
    pipe(this->descriptores);
}

void Pipe::setearModo(const int modo) {
    if (modo == LECTURA) {
        close(this->descriptores[1]);
        this->escritura = false;

    } else if (modo == ESCRITURA) {
        close(this->descriptores[0]);
        this->lectura = false;
    }
}

ssize_t Pipe::escribir(const void *dato, size_t datoSize) {
    if (this->lectura) {
        close(this->descriptores[0]);
        this->lectura = false;
    }

    return write(this->descriptores[1], dato, datoSize);
}

ssize_t Pipe::leer(void *buffer, size_t buffSize) {
    if (this->escritura) {
        close(this->descriptores[1]);
        this->escritura = false;
    }

    return read(this->descriptores[0], buffer, buffSize);
}

int Pipe::getFdLectura() const {
    if (this->lectura)
        return this->descriptores[0];
    else
        return -1;
}

int Pipe::getFdEscritura() const {
    if (this->escritura)
        return this->descriptores[1];
    else
        return -1;
}

void Pipe::cerrar() {
    if (this->lectura) {
        close(this->descriptores[0]);
        this->lectura = false;
    }

    if (this->escritura) {
        close(this->descriptores[1]);
        this->escritura = false;
    }
}