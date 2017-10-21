#ifndef BEACH_VOLEY_RESOURCEHANDLER_H
#define BEACH_VOLEY_RESOURCEHANDLER_H

#include <map>
#include "../ipc/Pipe.h"
#include "../ipc/Semaforo.h"
#include "../cons/Definition.h"

using namespace std;

class ResourceHandler {
private:
    static map<string, Semaforo> mSemaforo;
    static map<string, Pipe> mPipe;

    static void createSemaforo(string path, unsigned short initValue, int amount);

    static void createPipe(string pipeId);

public:
    static void init(Config config);

    static Semaforo *getSemaforo(string path);

    static Pipe *getPipe(string pipeId);

    static void freeResources();
};


#endif //BEACH_VOLEY_RESOURCEHANDLER_H
