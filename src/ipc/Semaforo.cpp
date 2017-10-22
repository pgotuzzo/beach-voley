#include "Semaforo.h"
#include <iostream>

Semaforo::Semaforo() = default;

Semaforo::Semaforo(const std::string &nombre, const ushort valorInicial, const int cantidadSemaforos) :
        valorInicial(valorInicial),
        count(cantidadSemaforos) {
    key_t clave = ftok(nombre.c_str(), 'a');
    if (clave == -1) {
        throw std::runtime_error("Couldn't create semapore: " + nombre);
    }
    this->id = semget(clave, cantidadSemaforos, 0666 | IPC_CREAT);

    int res = this->inicializar();
    if (res == -1) {
        throw std::runtime_error("Couldn't initialize semaphore: " + nombre);
    }
}

int Semaforo::inicializar() const {

    union semnum {
        int val;
        struct semid_ds *buf;
        ushort *array;
    };

    semnum init{};
    init.array = (ushort *) malloc(sizeof(ushort) * count);
    for (int i = 0; i < count; i++) {
        init.array[i] = valorInicial;
    }

    int resultado = semctl(this->id, 0, SETALL, init);
    return resultado;
}

int Semaforo::ultimoProceso() const {
    return semctl(this->id, 0, GETPID, 0);
}

int Semaforo::p(const unsigned short numeroSemaforo) const {

    struct sembuf operacion{};

    operacion.sem_num = numeroSemaforo;    // numero de semaforo
    operacion.sem_op = -1;    // restar 1 al semaforo
//    operacion.sem_flg = SEM_UNDO;

    int resultado = semop(this->id, &operacion, 1);
    return resultado;
}

int Semaforo::v(const unsigned short numeroSemaforo) const {

    struct sembuf operacion{};

    operacion.sem_num = numeroSemaforo;    // numero de semaforo
    operacion.sem_op = 1;    // sumar 1 al semaforo
//    operacion.sem_flg = SEM_UNDO;

    int resultado = semop(this->id, &operacion, 1);
    return resultado;
}

void Semaforo::eliminar() const {
    semctl(this->id, 0, IPC_RMID);
}

int Semaforo::getStatus(const unsigned short numeroSemaforo) const {
    return semctl(this->id, 0, GETVAL, 0);
}