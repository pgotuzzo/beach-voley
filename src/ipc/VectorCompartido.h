#ifndef VECTORCOMPARTIDO_H_
#define VECTORCOMPARTIDO_H_

#define SHM_OK            (0)
#define    ERROR_FTOK      (-1)
#define ERROR_SHMGET    (-2)
#define    ERROR_SHMAT     (-3)

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string>
#include <iostream>

template<class T>
class VectorCompartido {

private:
    int shmId;
    T *ptrDatos;
    int vectorSize;

    int cantidadProcesosAdosados() const;

public:
    VectorCompartido();

    int size();

    int crear(const std::string &archivo, char letra, int vectorSize);

    void liberar();

    void liberarForce();

    void escribir(const T &dato, unsigned int pos);

    T leer(unsigned int pos) const;

};

template<class T>
VectorCompartido<T>::VectorCompartido() : shmId(0), ptrDatos(NULL), vectorSize(0) {}

template<class T>
int VectorCompartido<T>::size() {
    return vectorSize;
}

template<class T>
int VectorCompartido<T>::crear(const std::string &archivo, const char letra, int vectorSize) {
    this->vectorSize = vectorSize;
    // generacion de la clave
    key_t clave = ftok(archivo.c_str(), letra);
    if (clave == -1) {
        return ERROR_FTOK;
    } else {
        // creacion de la memoria compartida
        this->shmId = shmget(clave, vectorSize * sizeof(T), 0644 | IPC_CREAT);

        if (this->shmId == -1) {
            return ERROR_SHMGET;
        } else {
            // attach del bloque de memoria al espacio de direcciones del proceso
            void *ptrTemporal = shmat(this->shmId, NULL, 0);

            if (ptrTemporal == (void *) -1) {
                return ERROR_SHMAT;
            } else {
                this->ptrDatos = static_cast<T *> (ptrTemporal);
                return SHM_OK;
            }
        }
    }
}


template<class T>
void VectorCompartido<T>::liberar() {
    // detach del bloque de memoria
    shmdt(static_cast<void *> (this->ptrDatos));

    int procAdosados = this->cantidadProcesosAdosados();

    if (procAdosados == 0) {
        shmctl(this->shmId, IPC_RMID, NULL);
    }
}

template<class T>
void VectorCompartido<T>::liberarForce() {
    // detach del bloque de memoria
    shmdt(static_cast<void *> (this->ptrDatos));
    shmctl(this->shmId, IPC_RMID, NULL);
}

template<class T>
void VectorCompartido<T>::escribir(const T &dato, unsigned int pos) {
    this->ptrDatos[pos] = dato;
}

template<class T>
T VectorCompartido<T>::leer(unsigned int pos) const {
    return this->ptrDatos[pos];
}

template<class T>
int VectorCompartido<T>::cantidadProcesosAdosados() const {
    shmid_ds estado{};
    shmctl(this->shmId, IPC_STAT, &estado);
    return static_cast<int>(estado.shm_nattch);
}


#endif /* VECTORCOMPARTIDO_H_ */