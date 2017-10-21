#include "Field.h"

Field::Field() = default;

Field::Field(int id, string name) : id(id), name(name) {}

bool Field::operator==(const Field &field) const {
    return id == field.id;
}

int Field::getId() {
    return id;
}


