#ifndef SIGINT_HANDLER_H_
#define SIGINT_HANDLER_H_

#include <signal.h>
#include <assert.h>

#include "EventHandler.h"
#include "../../src/ResourceHandler.h"

class SIGINT_Handler : public EventHandler {
private:
    sig_atomic_t gracefulQuit;

public:

    SIGINT_Handler() : gracefulQuit(0) {
    }

    ~SIGINT_Handler() {
    }

    virtual int handleSignal(int signum) {
        assert (signum == SIGINT);
        ResourceHandler::getInstance()->freeResources();
        return 0;
    }

};

#endif /* SIGINT_HANDLER_H_ */
