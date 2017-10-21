#include <sstream>
#include "ResourceHandler.h"
#include "FileUtils.h"
#include "../cons/Path.h"
#include "Logger.h"
#include "../cons/Constants.h"

static const string TAG = "Gestor de Recursos: ";

map<string, Semaforo> ResourceHandler::mSemaforo;
map<string, Pipe> ResourceHandler::mPipe;

void ResourceHandler::init(Config config) {
    Logger::d(TAG + "Inicializandose. Creando todos los recursos necesarios: Semaforos, Pipes, etc.");
    // Semaforos
    createSemaforo(PATH_SEM_TOURNAMENT_SUBSCRIPTION, 0, 1);
    createSemaforo(PATH_SEM_TOURNAMENT_START, 0, 1);
    createSemaforo(PATH_SEM_TURNSTILE, config.stadiumCapacity, 1);
    createSemaforo(PATH_SEM_FIELD_ENTRANCE, 0, config.rows * config.columns);
    createSemaforo(PATH_SEM_FIELD_EXIT, 0, config.rows * config.columns);
    // Pipes
    createPipe(PIPE_ID_MANAGER_QUEUE);
    for (const auto &playerName : config.vPlayerNames) {
        createPipe(PIPE_ID_RESPONSE_TO_PLYER_N + playerName);
    }
}

void ResourceHandler::createSemaforo(string path, unsigned short initValue, int amount) {
    createFileIfNotExists(path);
    Semaforo s(path, initValue, amount);
    mSemaforo.emplace(path, s);
}

void ResourceHandler::createPipe(string pipeId) {
    Pipe pipe;
    mPipe.emplace(pipeId, pipe);
}

Semaforo *ResourceHandler::getSemaforo(string path) {
    if (mSemaforo.find(path) == mSemaforo.end()) {
        throw runtime_error("Semaphore not available. Semaphore: " + path);
    }
    return &mSemaforo[path];
}

Pipe *ResourceHandler::getPipe(string pipeId) {
    if (mPipe.find(pipeId) == mPipe.end()) {
        throw runtime_error("Pipe not available. Pipe: " + pipeId);
    }
    return &mPipe[pipeId];
}

void ResourceHandler::freeResources() {
    Logger::d(TAG + "Liberando todos los recursos creados");
    for (auto semaforo : mSemaforo) {
        semaforo.second.eliminar();
    }
    for (auto pipe : mPipe) {
        pipe.second.cerrar();
    }
}


