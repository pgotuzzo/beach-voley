#ifndef SIGINT_HANDLER_H_
#define SIGINT_HANDLER_H_

#include <csignal>
#include <cassert>

#include "EventHandler.h"
#include "../../util/ResourceHandler.h"

class SIGINT_Handler : public EventHandler {
private:
    sig_atomic_t gracefulQuit;

public:

    SIGINT_Handler() : gracefulQuit(0) {
    }

    virtual int handleSignal(int signum) {
        assert (signum == SIGINT);
        ResourceHandler::freeResources();
        return 0;
    }

};

#endif /* SIGINT_HANDLER_H_ */
