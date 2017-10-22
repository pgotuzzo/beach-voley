#include "Field.h"

Field::Field() = default;

Field::Field(int id, int semId, string name) : id(id), semId(semId), name(name), state(FREE) {}

bool Field::operator==(const Field &field) const {
    return id == field.id;
}

int Field::getId() {
    return id;
}

int Field::getSemId() {
    return semId;
}

string Field::getName() {
    return name;
}

Field::State Field::getState() {
    return state;
}

void Field::setState(State state) {
    this->state = state;
}

void Field::setMatch(Match match) {
    this->match = match;
}

Match Field::getMatch() {
    return match;
}


