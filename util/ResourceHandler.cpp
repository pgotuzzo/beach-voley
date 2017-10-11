#include <sstream>
#include <iostream>
#include <cstring>
#include "ResourceHandler.h"
#include "../src/config/Constants.h"

// TODO - Move to a FileUtils class
void createFileIfNotExist(string path) {
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

void deleteFile(string path) {
    if (remove(path.c_str()) != 0) {
//        stringstream message;
        cout << "The file: " << path << " couldn't be deleted! Error Number: " << strerror(errno) << " " << errno
             << endl;
//        throw runtime_error(message.str());
    }
}

ResourceHandler *ResourceHandler::instance = nullptr;
map<string, Semaforo> ResourceHandler::mSemaforo;
map<string, FifoWrite> ResourceHandler::mFifoWrite;
map<string, FifoRead> ResourceHandler::mFifoRead;

ResourceHandler::ResourceHandler() = default;

ResourceHandler *ResourceHandler::getInstance() {
    if (instance == nullptr) {
        instance = new ResourceHandler();
    }
    return instance;
}

void ResourceHandler::init(Config config) {
    // FIFOs
    //      Manager
    string path = FIFO_FILE_MANAGER_RECEIVE_TASK;
    FifoWrite fifoWriteTask(path);
    mFifoWrite.emplace(path, fifoWriteTask);
    FifoRead fifoReadTask(path);
    mFifoRead.emplace(path, fifoReadTask);
    //      Players
    for (int i = 0; i < config.tournamentParams.players.size(); i++) {
        path = FIFO_FILE_PARTNER_RESPONSE + to_string(i);
        FifoWrite fifoPlayerWrite(path);
        mFifoWrite.emplace(path, fifoPlayerWrite);
        FifoRead fifoPlayerRead(path);
        mFifoRead.emplace(path, fifoPlayerRead);
    }
    // Semaphore
    //      Stadium turnstile
    path = SEM_TURNSTILE;
    createFileIfNotExist(path);
    Semaforo s(path, config.tournamentParams.capacity, 1);
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
}

Semaforo *ResourceHandler::getSemaforo(string path) {
    if (mSemaforo.find(path) == mSemaforo.end()) {
        throw runtime_error("Semaphore not available. Semaphore: " + path);
    }
    return &mSemaforo[path];
}

FifoRead *ResourceHandler::getFifoRead(string path) {
    if (mFifoRead.find(path) == mFifoRead.end()) {
        throw runtime_error("Fifo Read not available. Fifo: " + path);
    }
    return &mFifoRead[path];
}

FifoWrite *ResourceHandler::getFifoWrite(string path) {
    if (mFifoWrite.find(path) == mFifoWrite.end()) {
        throw runtime_error("Fifo Write not available. Fifo: " + path);
    }
    return &mFifoWrite[path];
}

VectorCompartido<int> ResourceHandler::createVectorCompartido(string path, char aChar, size_t vectorSize) {
    createFileIfNotExist(path);
    VectorCompartido<int> vectorCompartido;
    vectorCompartido.crear(path, aChar, vectorSize);
    return vectorCompartido;
}

void ResourceHandler::freeResources() {
    cout << endl << "Liberando recursos" << endl;
    for (auto item : mSemaforo) {
        item.second.eliminar();
    }
    for (auto item : mFifoRead) {
        item.second.closeFifo();
        item.second.deleteFifo();
    }
    for (auto item : mFifoWrite) {
        item.second.closeFifo();
        item.second.deleteFifo();
    }
}




