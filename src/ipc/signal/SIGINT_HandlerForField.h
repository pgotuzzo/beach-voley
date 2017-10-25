#ifndef SIGINT_HANDLERFORFIELD_H_
#define SIGINT_HANDLERFORFIELD_H_

#include <csignal>
#include <cassert>

#include "EventHandler.h"
#include "../../stadium/FieldProcess.h"

class SIGINT_HandlerForField : public EventHandler {
private:
    FieldProcess *associatedFieldProcess;

public:

    explicit SIGINT_HandlerForField(FieldProcess *associatedFieldProcess) :
            associatedFieldProcess(associatedFieldProcess) {}

    /**
     * Inmediatly release the players from the field if there is any, updates the status of the field
     * and sends the notification to the manager.
     * @param signum the received signal
     */
    virtual int handleSignal(int signum) {
        assert (signum == SIGINT);
        associatedFieldProcess->toggleTide();
        return 0;
    }

};

#endif /* SIGINT_HANDLERFORFIELD_H_ */
