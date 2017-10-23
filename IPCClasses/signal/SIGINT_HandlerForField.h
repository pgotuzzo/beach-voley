#ifndef SIGINT_HANDLERFORFIELD_H_
#define SIGINT_HANDLERFORFIELD_H_

#include <csignal>
#include <cassert>

#include "EventHandler.h"
#include "../../src/stadium/Field.h"

class SIGINT_HandlerForField : public EventHandler {
private:
    Field *associatedField;

public:

    explicit SIGINT_HandlerForField(Field *associatedField) : associatedField(associatedField) {
    }

    /**
     * Inmediatly release the players from the field if there is any, updates the status of the field
     * and sends the notification to the manager.
     * @param signum the received signal
     */
    virtual int handleSignal(int signum) {
        assert (signum == SIGINT);
        associatedField->releasePlayersAndEndGame();
        associatedField->toggleFloodedAndSendNotification();
        return 0;
    }

};

#endif /* SIGINT_HANDLERFORFIELD_H_ */
