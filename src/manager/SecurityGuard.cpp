#include "SecurityGuard.h"
#include "../util/Logger.h"
#include "../util/PlayerUtils.h"
#include "../util/RandomNumber.h"

const string TAG = "Agente de Seguridad: ";

SecurityGuard::SecurityGuard(vector<Player> *players) : vPlayers(players) {}

void SecurityGuard::dismissPlayer(Player player, DismissAction action) {
    OrgPlayerResponse response{};
    response.fieldId = 0;
    response.playerAction = (action == DismissAction::STADIUM ? PlayerAction::LEAVE_STADIUM
                                                              : PlayerAction::LEAVE_TOURNAMENT);

    Pipe *pipe = player.getPipe();
    pipe->setearModo(Pipe::ESCRITURA);
    ssize_t out = pipe->escribir(&response, sizeof(OrgPlayerResponse));
    if (out < 0) {
        Logger::e(TAG + " fallo el envio de una respuesta!!!");
        throw runtime_error(TAG + " fallo el envio de una respuesta!!!");
    }
    Logger::d(TAG + "Mensaje enviado correctamente a " + player.getName() + ".\n" + response.toString());
}

void SecurityGuard::dismissPlayerFromStadium() {
    vector<Player> playersWaiting = findPlayersByState(*vPlayers, Player::State::WAITING_FOR_PARTNER);
    int randomIdx = getRandomInt(0, static_cast<int>(playersWaiting.size() - 1));
    Player playerToDismiss = playersWaiting[randomIdx];
    Logger::d(TAG + "Se despide a " + playerToDismiss.getName() + " del predio");

    dismissPlayer(playerToDismiss, STADIUM);

    // Updates player state to OUTSIDE
    int idx = findPlayerById(*vPlayers, playerToDismiss.getId());
    vPlayers->at(idx).setState(Player::State::OUTSIDE);
}

void SecurityGuard::dismissPlayerFromTournament(int playerId) {
    int idx = findPlayerById(*vPlayers, playerId);
    Player playerToDismiss = vPlayers->at(idx);
    Logger::d(TAG + "Se despide a " + playerToDismiss.getName() + " del torneo. Quedan " + to_string(vPlayers->size()));

    dismissPlayer(playerToDismiss, TOURNAMENT);

    // Removes player
    vPlayers->erase(find(vPlayers->begin(), vPlayers->end(), playerToDismiss));
    // Removes player as a possible partner of remaining players
    for (auto &vPlayer : *vPlayers) {
        vPlayer.removePotentialPartner(playerId);
    }
}

void SecurityGuard::dismissPlayersWaitingFromTournament() {
    Logger::d(TAG + "Se despide a todos los jugadores en espera de pareja u oponente del torneo");
    // Players who don't have a partner assigned
    vector<Player> playersWaiting = findPlayersByState(*vPlayers, Player::State::WAITING_FOR_PARTNER);
    for (Player p : playersWaiting) {
        dismissPlayerFromTournament(p.getId());
    }
    // Players who have a partner (team made) but haven't found an opponent
    vector<Player> playersWaitingForOpponent = findPlayersByState(*vPlayers, Player::State::PARTNER_ASSIGNED);
    for (Player p : playersWaitingForOpponent) {
        dismissPlayerFromTournament(p.getId());
    }
    // Log status
    stringstream streamPlayers;
    for (Player p : *vPlayers) {
        streamPlayers << p.getName() << endl;
    }
    Logger::d(TAG + "Quedan en el torneo los jugadores que aun estan jugando: \n" + streamPlayers.str());
}
