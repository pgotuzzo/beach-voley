#ifndef PIPE_H_
#define PIPE_H_

#include <unistd.h>
#include <fcntl.h>

class Pipe {

private:
    int descriptores[2];
    bool lectura;
    bool escritura;

public:
    static const int LECTURA = 0;
    static const int ESCRITURA = 1;

    Pipe();

    void setearModo(int modo);

    ssize_t escribir(const void *dato, size_t datoSize);

    ssize_t leer(void *buffer, size_t buffSize);

    int getFdLectura() const;

    int getFdEscritura() const;

    void cerrar();
};

#endif /* PIPE_H_ */