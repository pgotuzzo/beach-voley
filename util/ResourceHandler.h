#ifndef BEACH_VOLEY_RESOURCEHANDLER_H
#define BEACH_VOLEY_RESOURCEHANDLER_H

#include <map>
#include "../IPCClasses/Semaforo.h"
#include "../IPCClasses/fifo/FifoRead.h"
#include "../IPCClasses/fifo/FifoWrite.h"
#include "../IPCClasses/VectorCompartido.h"
#include "../src/config/Config.h"

using namespace std;

class ResourceHandler {
private:
    static ResourceHandler *instance;
    static map<string, Semaforo> mSemaforo;
    static map<string, FifoRead> mFifoRead;
    static map<string, FifoWrite> mFifoWrite;

    ResourceHandler();

public:
    static ResourceHandler *getInstance();

    static void init(Config config);

    Semaforo *getSemaforo(string path);

    FifoRead *getFifoRead(string path);

    FifoWrite *getFifoWrite(string path);

    VectorCompartido<int> createVectorCompartido(string path, char aChar, size_t vectorSize);

    void freeResources();
};


#endif //BEACH_VOLEY_RESOURCEHANDLER_H
