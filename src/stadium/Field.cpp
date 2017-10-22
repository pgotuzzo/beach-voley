#include "Field.h"

Field::Field() = default;

Field::Field(int id, string name) : id(id), name(name), state(FREE) {}

bool Field::operator==(const Field &field) const {
    return id == field.id;
}

int Field::getId() {
    return id;
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

