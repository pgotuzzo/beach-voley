#ifndef SEMAFORO_H_
#define SEMAFORO_H_

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <string>

class Semaforo {

private:
    int id;
    ushort valorInicial;
    int count;

    int inicializar() const;

public:
    Semaforo();

    Semaforo(const std::string &nombre, ushort valorInicial, int cantidadSemaforos);

    int p(unsigned short numeroSemaforo) const; // decrementa
    int v(unsigned short numeroSemaforo) const; // incrementa
    int ultimoProceso() const; // pid del ultimo proceso que uso el sem
    void eliminar() const;
    int getStatus(unsigned short numeroSemaforo) const;
};

struct SemaforoInfo {
    unsigned short id;
    Semaforo *s;
};

#endif /* SEMAFORO_H_ */