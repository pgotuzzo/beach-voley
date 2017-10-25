#include "PlayerProcess.h"
#include "../util/Logger.h"
#include "../util/RandomNumber.h"

PlayerProcess::PlayerProcess(string playerName, Semaforo *tournamentSubscription, Semaforo *tournamentStart,
                             Semaforo *stadiumEntrance, Semaforo *fieldEntrance, Semaforo *fieldExit,
                             Pipe *pipeToManager, Pipe *pipeFromManager, Stadium *stadium) :
        playerName(playerName),
        tournamentSubscription(tournamentSubscription),
        tournamentStart(tournamentStart),
        stadiumEntrance(stadiumEntrance),
        fieldEntrance(fieldEntrance),
        fieldExit(fieldExit),
        pipeToManager(pipeToManager),
        pipeFromManager(pipeFromManager),
        stadium(stadium) {}

int PlayerProcess::start() {
    int pid = fork();
    if (pid == 0) {
        subscribeToTournament();
        while (!leaveTournament) {
            stadiumEntrance->p(0);
            leaveStadium = false;
            Logger::d(playerName + " ingresa al predio");
            while (!leaveStadium && !leaveTournament) {
                play();
            }
            Logger::d(playerName + " abandona el predio");
            stadiumEntrance->v(0);
            usleep(getRandomUnsignedInt(5000, 10000));
        }
        Logger::d(playerName + " se retira del torneo");
        exit(0);
    }
    return pid;
}

void PlayerProcess::subscribeToTournament() {
    Logger::d(playerName + " se acaba de inscribir en el torneo!");
    tournamentSubscription->v(0);
    tournamentStart->p(0);
    Logger::d(playerName + " comenzo a participar en el torneo!");
}

void PlayerProcess::play() {
    sendFindPartnerRequest();
    OrgPlayerResponse response = receiveFindPartnerResponse();
    switch (response.playerAction) {
        case PLAY:
            goToField(response.fieldId);
            break;
        case LEAVE_STADIUM:
            leaveStadium = true;
            break;
        case LEAVE_TOURNAMENT:
            leaveTournament = true;
            break;
    }
}

void PlayerProcess::sendFindPartnerRequest() {
    TaskRequest request{};
    request.pid = getpid();
    request.task = FIND_PARTNER;

    pipeToManager->setearModo(Pipe::ESCRITURA);
    ssize_t out = pipeToManager->escribir(&request, sizeof(TaskRequest));
    if (out < 0) {
        Logger::e(playerName + " fallo el envio de la solicitud para hallar un compañero!!! ");
        throw runtime_error(playerName + " fallo el envio de la solicitud para hallar un compañero!!! ");
    }
    Logger::d(playerName + " buscando compañero");
}

OrgPlayerResponse PlayerProcess::receiveFindPartnerResponse() {
    OrgPlayerResponse response{};

    ssize_t out = pipeFromManager->leer(&response, sizeof(OrgPlayerResponse));
    if (out < 0) {
        Logger::e(playerName + " fallo la recepcion de la respuesta para hallar un compañero!!! ");
        throw runtime_error(playerName + " fallo la recepcion de la respuesta para hallar un compañero!!! ");
    }
    Logger::d(playerName + " recibio una respuesta del Organizador:\n" + response.toString());
    return response;
}

void PlayerProcess::goToField(int fieldId) {
    int semId = stadium->getFieldById(fieldId)->getSemId();

    fieldEntrance->v(semId);
    Logger::d(playerName + " ingreso a la Cancha " + stadium->getFieldById(fieldId)->getName());
    fieldExit->p(semId);
    Logger::d(playerName + " salio de la Cancha " + stadium->getFieldById(fieldId)->getName());
}