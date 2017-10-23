#ifndef LOCKFILE_H_
#define LOCKFILE_H_

#include <unistd.h>
#include <fcntl.h>
#include <string>

class LockFile {

private:
    struct flock fl;
    int fd;
    std::string nombre;

public:
    explicit LockFile(std::string nombre);

    ~LockFile();

    int tomarLockEscritura();

    int tomarLockLectura();

    int liberarLock();

    ssize_t escribir(const void *buffer, size_t buffsize) const;
};

#endif /* LOCKFILE_H_ */