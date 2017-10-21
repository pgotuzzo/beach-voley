#include "MatchMaker.h"
#include "../util/PlayerUtils.h"
#include "../util/Logger.h"

const string TAG = "Coordinador: ";

MatchMaker::MatchMaker(vector<Player> *players) : vPlayers(players) {}

void MatchMaker::releasePlayersWithoutMatch() {
    // Release players from teams in construction
    local.ready = false;
    visitant.ready = false;
}

bool MatchMaker::findPartnerAvailable(int playerId) {
    // FIXME - Mock begin
    // return false;
    // FIXME - Mock end

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
                vPlayers->at(findPlayerById(*vPlayers, visitant.firstPlayerId)).setState(Player::State::READY_FOR_MATCH);
                vPlayers->at(findPlayerById(*vPlayers, visitant.secondPlayerId)).setState(Player::State::READY_FOR_MATCH);
                // Free teams attributes
                local.ready = false;
                visitant.ready = false;

                Logger::d(TAG + "Se tienen dos parejas listas para jugar un partido");

            } else {
                // Create local team
                local.firstPlayerId = playerId;
                local.secondPlayerId = potentialPartnerId;
                local.ready = true;
                // Update players state
                cout << " PUTO";
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
    // TODO
}

