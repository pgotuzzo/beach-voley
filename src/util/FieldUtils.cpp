#include <vector>
#include "FieldUtils.h"

int findFieldById(vector<Field> fields, int fieldId) {
    for (int i = 0; i < fields.size(); i++) {
        Field f = fields[i];
        if (f.getId() == fieldId) {
            return i;
        }
    }
    return -1;
}

vector<int> findFieldsByState(vector<Field> fields, Field::State state) {
    vector<int> matches;
    for (int i = 0; i < fields.size(); i++) {
        if (fields[i].getState() == state) {
            matches.push_back(i);
        }
    }
    return matches;
}