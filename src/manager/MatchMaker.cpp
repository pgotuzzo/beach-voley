#include "MatchMaker.h"
#include "../util/PlayerUtils.h"
#include "../util/Logger.h"

const string TAG = "Coordinador: ";

MatchMaker::MatchMaker(vector<Player> *players, Stadium *stadium) : vPlayers(players), stadium(stadium) {
    local.ready = false;
    visitant.ready = false;
}

void MatchMaker::releasePlayersWithoutMatch() {
    // Release players from teams in construction
    local.ready = false;
    visitant.ready = false;
}

bool MatchMaker::findPartnerAvailable(int playerId) {
    int idx = findPlayerById(*vPlayers, playerId);
    Player player = vPlayers->at(idx);

    Logger::d(TAG + "Buscando pareja para " + player.getName());

    for (int potentialPartnerId : player.getPotentialPartners()) {
        // For each potential partner check if it is waiting for a partner too
        int partnerIdx = findPlayerById(*vPlayers, potentialPartnerId);
        if (vPlayers->at(partnerIdx).getState() == Player::State::WAITING_FOR_PARTNER) {

            Logger::d(TAG + "Se encontro pareja para " + player.getName() + ". Pareja asignada: " +
                      vPlayers->at(partnerIdx).getName());

            // Found a partner, now update the teams and create a match if it is necessary
            if (local.ready) {
                // Create visitant team
                visitant.firstPlayerId = playerId;
                visitant.secondPlayerId = potentialPartnerId;
                // Create match
                Match match = {local, visitant};
                matchesWaitingForField.push_back(match);
                // Updates players state
                vPlayers->at(findPlayerById(*vPlayers, local.firstPlayerId)).setState(Player::State::READY_FOR_MATCH);
                vPlayers->at(findPlayerById(*vPlayers, local.secondPlayerId)).setState(Player::State::READY_FOR_MATCH);
                vPlayers->at(findPlayerById(*vPlayers, visitant.firstPlayerId)).setState(
                        Player::State::READY_FOR_MATCH);
                vPlayers->at(findPlayerById(*vPlayers, visitant.secondPlayerId)).setState(
                        Player::State::READY_FOR_MATCH);
                // Free teams attributes
                local.ready = false;
                visitant.ready = false;

                // Log
                stringstream streamMatch;
                streamMatch << "\tPartido: " << endl
                            << "\t\tLocal: " << vPlayers->at(findPlayerById(*vPlayers, local.firstPlayerId)).getName()
                            << " - " << vPlayers->at(findPlayerById(*vPlayers, local.secondPlayerId)).getName() << endl
                            << "\t\tVisitant: "
                            << vPlayers->at(findPlayerById(*vPlayers, visitant.firstPlayerId)).getName() << " - "
                            << vPlayers->at(findPlayerById(*vPlayers, visitant.secondPlayerId)).getName() << endl;
                Logger::d(TAG + "Se tienen dos parejas listas para jugar un partido. \n" + streamMatch.str());

            } else {
                // Create local team
                local.firstPlayerId = playerId;
                local.secondPlayerId = potentialPartnerId;
                local.ready = true;
                // Update players state
                vPlayers->at(findPlayerById(*vPlayers, local.firstPlayerId)).setState(Player::State::PARTNER_ASSIGNED);
                vPlayers->at(findPlayerById(*vPlayers, local.secondPlayerId)).setState(Player::State::PARTNER_ASSIGNED);

                Logger::d(TAG + "Se encontro una pareja...ahora a esperar por un oponente.");

            }
            return true;
        }
    }
    Logger::d(TAG + "No se encontro pareja para " + player.getName());
    return false;
}

void MatchMaker::checkFieldAssignation() {
    // No matches to be played
    if (matchesWaitingForField.empty()) {
        Logger::d(TAG + "No hay partidos pendientes...");
        return;
    }

    vector<Field> freeFields = stadium->getFieldsByState(Field::State::FREE);
    if (!freeFields.empty()) {
        int fieldId = freeFields[0].getId();
        Logger::d(TAG + "Hay partidos pendientes, y al menos una cancha para utilizar!!! Cancha " +
                  stadium->getFieldById(fieldId)->getName());
        assignField(fieldId);
    } else {
        Logger::d(TAG + "Hay partidos pendientes, pero NINGUNA cancha disponible");
    }
}

void MatchMaker::assignField(int fieldId) {
    Logger::d(
            TAG + "Se asigna la Cancha " + stadium->getFieldById(fieldId)->getName() + " para el partido pendiente!!!");
    // Pop the first pending match
    Match match = *matchesWaitingForField.begin();
    // Remove from pending list
    matchesWaitingForField.erase(matchesWaitingForField.begin());
    // Update stadium/field status
    stadium->getFieldById(fieldId)->setState(Field::State::FULL);
    stadium->getFieldById(fieldId)->setMatch(match);
    // Send each player to the field
    sendPlayerToField(fieldId, match.local.firstPlayerId);
    sendPlayerToField(fieldId, match.local.secondPlayerId);
    sendPlayerToField(fieldId, match.visitant.firstPlayerId);
    sendPlayerToField(fieldId, match.visitant.secondPlayerId);
}

void MatchMaker::sendPlayerToField(int fieldId, int playerId) {
    OrgPlayerResponse response{};
    response.fieldId = fieldId;
    response.playerAction = PlayerAction::PLAY;

    int idx = findPlayerById(*vPlayers, playerId);
    Pipe *pipe = vPlayers->at(idx).getPipe();
    pipe->setearModo(Pipe::ESCRITURA);
    ssize_t out = pipe->escribir(&response, sizeof(OrgPlayerResponse));
    if (out < 0) {
        Logger::e(TAG + " fallo el envio de una respuesta!!!");
        throw runtime_error(TAG + " fallo el envio de una respuesta!!!");
    }
    Logger::d(TAG + "Mensaje enviado correctamente a " + vPlayers->at(idx).getName() + ".\n" + response.toString());
}

