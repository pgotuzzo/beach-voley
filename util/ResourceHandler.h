#ifndef BEACH_VOLEY_RESOURCEHANDLER_H
#define BEACH_VOLEY_RESOURCEHANDLER_H

#include <map>
#include "../IPCClasses/Semaforo.h"
#include "../IPCClasses/VectorCompartido.h"
#include "../src/config/Config.h"
#include "../IPCClasses/Pipe.h"

using namespace std;

class ResourceHandler {
private:
    static ResourceHandler *instance;
    static map<string, Semaforo> mSemaforo;
    static map<string, VectorCompartido<int>> mVectorCompartido;

    ResourceHandler();

public:
    static ResourceHandler *getInstance();

    static void init(Config config);

    Semaforo *getSemaforo(string path);

    VectorCompartido<int> *getVectorCompartido(string path);

    void freeResources();
};


#endif //BEACH_VOLEY_RESOURCEHANDLER_H
