#ifndef SCOREBOARD_SIGINT_HANDLER_H_
#define SCOREBOARD_SIGINT_HANDLER_H_

#include <csignal>
#include <cassert>

#include "../ipc/signal/EventHandler.h"
#include "../player/Player.h"
#include "../ipc/VectorCompartido.h"

class Scoreboard_SIGINT_Handler : public EventHandler {
private:
    sig_atomic_t gracefulQuit;
    bool *quit;

public:

    Scoreboard_SIGINT_Handler(bool *quit) : gracefulQuit(0), quit(quit) {
    }

    virtual int handleSignal(int signum) {
        assert (signum == SIGINT);
        *quit = true;
        Logger::d("Scoreboard Handler: Terminando!!");
        return 0;
    }

};

#endif /* SCOREBOARD_SIGINT_HANDLER_H_ */
