#ifndef SIGTERM_HANDLER_H_
#define SIGTERM_HANDLER_H_

#include <csignal>
#include <cassert>

#include "EventHandler.h"
#include "../../src/util/ResourceHandler.h"

class SIGTERM_Handler : public EventHandler {
private:
    sig_atomic_t gracefulQuit;

public:

    SIGTERM_Handler() : gracefulQuit(0) {
    }

    virtual int handleSignal(int signum) {
        assert (signum == SIGTERM);
        this->gracefulQuit = 1;
        return 0;
    }

    sig_atomic_t getGracefulQuit() const {
        return this->gracefulQuit;
    }
};

#endif /* SIGTERM_HANDLER_H_ */
