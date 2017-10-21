#ifndef BEACH_VOLEY_AUX_PLAYERUTILS_H
#define BEACH_VOLEY_AUX_PLAYERUTILS_H


#include "../player/Player.h"

int findPlayerById(vector<Player> players, int playerId);

vector<Player> findPlayersByState(vector<Player> players, Player::State playerState);

#endif //BEACH_VOLEY_AUX_PLAYERUTILS_H
