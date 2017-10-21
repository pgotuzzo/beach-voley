#include <fcntl.h>
#include <sstream>
#include <zconf.h>
#include "FileUtils.h"

void createFileIfNotExists(string path) {
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
