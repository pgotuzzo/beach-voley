#include <sstream>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include "ResourceHandler.h"
#include "../config/Constants.h"

// TODO - Move to a FileUtils class
void createFileIfNotExist(const string &path) {
    int fd = open(path.c_str(), O_RDWR | O_CREAT, S_IRWXU);
    if (fd < 0) {
        stringstream message;
        message << "The file: " << path << " couldn't be created/opened! Error Number: " << strerror(errno) << " "
                << errno;
        // TODO - Create exception
        throw runtime_error(message.str());
    }
    close(fd);
}

ResourceHandler *ResourceHandler::instance = nullptr;
map<string, Semaforo> ResourceHandler::mSemaforo;
map<string, VectorCompartido<int>> ResourceHandler::mVectorCompartido;

ResourceHandler::ResourceHandler() = default;

ResourceHandler *ResourceHandler::getInstance() {
    if (instance == nullptr) {
        instance = new ResourceHandler();
    }
    return instance;
}

void ResourceHandler::init(Config config) {
    //      Manager
    string path = FIFO_FILE_MANAGER_RECEIVE_TASK;
    // Semaphore
    //      Stadium turnstile
    path = SEM_TURNSTILE;
    createFileIfNotExist(path);
    Semaforo s(path, static_cast<ushort>(config.tournamentParams.capacity), 1);
    mSemaforo.emplace(path, s);
    //      Field entrance
    path = SEM_FILE_FIELD_ENTRANCE;
    createFileIfNotExist(path);
    s = Semaforo(path, 0, config.tournamentParams.rows * config.tournamentParams.columns);
    mSemaforo.emplace(path, s);
    //      Field exit
    path = SEM_FILE_FIELD_EXIT;
    createFileIfNotExist(path);
    s = Semaforo(path, 0, config.tournamentParams.rows * config.tournamentParams.columns);
    mSemaforo.emplace(path, s);
    // Shared Memory
    //      IDs Vector
    path = SHARED_MEMORY_IDS_VECTOR;
    createFileIfNotExist(path);
    VectorCompartido<int> vectorCompartido1;
    vectorCompartido1.crear(path, 'a', config.tournamentParams.players.size());
    mVectorCompartido.emplace(path, vectorCompartido1);
    //      Points Vector
    path = SHARED_MEMORY_POINTS_VECTOR;
    createFileIfNotExist(path);
    VectorCompartido<int> vectorCompartido2;
    vectorCompartido2.crear(path, 'a', config.tournamentParams.players.size());
    mVectorCompartido.emplace(path, vectorCompartido2);
}

Semaforo *ResourceHandler::getSemaforo(string path) {
    if (mSemaforo.find(path) == mSemaforo.end()) {
        throw runtime_error("Semaphore not available. Semaphore: " + path);
    }
    return &mSemaforo[path];
}

VectorCompartido<int> *ResourceHandler::getVectorCompartido(string path) {
    if (mVectorCompartido.find(path) == mVectorCompartido.end()) {
        throw runtime_error("Vector compartido not available. Vector compartido: " + path);
    }
    return &mVectorCompartido[path];
}

void ResourceHandler::freeResources() {
    cout << endl << "Liberando recursos" << endl;
    for (auto item : mSemaforo) {
        item.second.eliminar();
    }
    for (auto item : mVectorCompartido) {
        item.second.liberar();
        remove(item.first.c_str());
    }
}


