#include <sstream>
#include <iostream>
#include "ResourceHandler.h"

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
        stringstream message;
        message << "The file: " << path << " couldn't be deleted! Error Numer: " << strerror(errno) << " " << errno;
        throw runtime_error(message.str());
    }
}

ResourceHandler *ResourceHandler::instance = nullptr;

ResourceHandler::ResourceHandler() = default;

ResourceHandler *ResourceHandler::getInstance() {
    if (instance == nullptr) {
        instance = new ResourceHandler();
    }
    return instance;
}

Semaforo *ResourceHandler::createSemaforo(string path, int initialValue, int count) {
    if (mSemaforo.find(path) == mSemaforo.end()) {
        createFileIfNotExist(path);
        Semaforo s(path, initialValue, count);
        mSemaforo.emplace(path, s);
    }
    return &mSemaforo[path];
}

FifoRead *ResourceHandler::createFifoRead(string path) {
    if (mFifoRead.find(path) == mFifoRead.end()) {
        FifoRead f(path);
        mFifoRead.emplace(path, f);
    }
    return &mFifoRead[path];
}

FifoWrite *ResourceHandler::createFifoWrite(string path) {
    if (mFifoWrite.find(path) == mFifoWrite.end()) {
        FifoWrite f(path);
        mFifoWrite.emplace(path, f);
    }
    return &mFifoWrite[path];
}

void ResourceHandler::freeResources() {
    cout << endl << "Liberando recursos" << endl;
    for (auto item : mSemaforo) {
        item.second.eliminar();
    }
    for (auto item : mFifoRead) {
        item.second.closeFifo();
        item.second.deleteFifo();
        deleteFile(item.first);
    }
    for (auto item : mFifoWrite) {
        item.second.closeFifo();
        item.second.deleteFifo();
        deleteFile(item.first);
    }
}




