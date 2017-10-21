#include "PlayerUtils.h"

int findPlayerById(vector<Player> players, int playerId) {
    for (int i = 0; i < players.size(); i++) {
        Player p = players[i];
        if (p.getId() == playerId) {
            return i;
        }
    }
    return -1;
}

vector<Player> findPlayersByState(vector<Player> players, Player::State playerState) {
    vector<Player> matches;
    for (Player p : players) {
        if (p.getState() == playerState) {
            matches.push_back(p);
        }
    }
    return matches;
}
