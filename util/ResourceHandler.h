#ifndef BEACH_VOLEY_RESOURCEHANDLER_H
#define BEACH_VOLEY_RESOURCEHANDLER_H

#include <map>
#include "../IPCClasses/Semaforo.h"
#include "../IPCClasses/fifo/FifoRead.h"
#include "../IPCClasses/fifo/FifoWrite.h"
#include "../IPCClasses/VectorCompartido.h"

using namespace std;

class ResourceHandler {
private:
    static ResourceHandler *instance;

    map<string, Semaforo> mSemaforo;
    map<string, FifoRead> mFifoRead;
    map<string, FifoWrite> mFifoWrite;

    ResourceHandler();

public:
    static ResourceHandler *getInstance();

    Semaforo *createSemaforo(string path, int initialValue, int count);

    FifoRead *createFifoRead(string path);

    FifoWrite *createFifoWrite(string path);

    VectorCompartido<int> createVectorCompartido(string path, char aChar, size_t vectorSize);

    void freeResources();
};


#endif //BEACH_VOLEY_RESOURCEHANDLER_H
