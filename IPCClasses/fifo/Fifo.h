#ifndef BEACH_VOLEY_FIFO_H
#define BEACH_VOLEY_FIFO_H

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

class Fifo {
public:
    Fifo();

    Fifo(const std::string name);

    virtual ~Fifo();

    virtual int openFifo() = 0;

    void closeFifo();

    void deleteFifo() const;

protected:
    std::string name;
    int fd;
};


#endif //BEACH_VOLEY_FIFO_H
