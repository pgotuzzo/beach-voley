#include "SignalHandler.h"

SignalHandler *SignalHandler::instance = nullptr;
EventHandler *SignalHandler::signal_handlers[NSIG];

SignalHandler *SignalHandler::getInstance() {

    if (instance == nullptr)
        instance = new SignalHandler();

    return instance;
}

void SignalHandler::destruir() {
    if (instance != nullptr) {
        delete (instance);
        instance = nullptr;
    }
}

EventHandler *SignalHandler::registrarHandler(int signum, EventHandler *eh) {

    EventHandler *old_eh = SignalHandler::signal_handlers[signum];
    SignalHandler::signal_handlers[signum] = eh;

    struct sigaction sa{};
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SignalHandler::dispatcher;
    sigemptyset(
            &sa.sa_mask);    // inicializa la mascara de seniales a bloquear durante la ejecucion del handler como vacio
    sigaddset(&sa.sa_mask, signum);
    sigaction(signum, &sa, nullptr);    // cambiar accion de la senial

    return old_eh;
}

void SignalHandler::dispatcher(int signum) {

    if (SignalHandler::signal_handlers[signum] != nullptr)
        SignalHandler::signal_handlers[signum]->handleSignal(signum);
}

int SignalHandler::removerHandler(int signum) {

    SignalHandler::signal_handlers[signum] = nullptr;
    return 0;
}
