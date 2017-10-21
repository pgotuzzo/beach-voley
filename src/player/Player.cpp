
#include "Player.h"

Player::Player(int id, string name, Pipe *pipe) : id(id), name(name), state(OUTSIDE), pipe(pipe), matchesCount(0) {}

bool Player::operator==(const Player &player) const {
    return this->id == player.id;
}

int Player::getId() {
    return id;
}

string Player::getName() {
    return name;
}

void Player::setState(Player::State state) {
    this->state = state;
}

Player::State Player::getState() {
    return state;
}

Pipe *Player::getPipe() {
    return pipe;
}

void Player::addPotentialPartner(int playerId) {
    auto match = find(potentialPartners.begin(), potentialPartners.end(), playerId);
    if (match == potentialPartners.end()) {
        potentialPartners.push_back(playerId);
    }
}

vector<int> Player::getPotentialPartners() {
    return potentialPartners;
}

void Player::removePotentialPartner(int playerId) {
    auto match = find(potentialPartners.begin(), potentialPartners.end(), playerId);
    if (match != potentialPartners.end()) {
        potentialPartners.erase(match);
    }
}

int Player::getMatchesCount() {
    return matchesCount;
}

void Player::increaseMatchesCount() {
    matchesCount++;
}
