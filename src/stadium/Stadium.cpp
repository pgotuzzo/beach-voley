#include "Stadium.h"
#include "../util/FieldUtils.h"

Stadium::Stadium(int rows, int columns, vector<Field> vFields) :
        rows(rows), columns(columns), vFields(vFields) {}

Field *Stadium::getFieldById(int fieldId) {
    int idx = findFieldById(vFields, fieldId);
    if (idx >= 0) {
        return &vFields[idx];
    }
    return nullptr;
}

vector<Field> Stadium::getFieldsByState(Field::State state) {
    vector<int> fieldsIdxs = findFieldsByState(vFields, state);
    vector<Field> fields;
    for (int idx : fieldsIdxs) {
        Field f = vFields[idx];
        fields.push_back(f);
    }
    return fields;
}

int Stadium::getColumnsCount() {
    return columns;
}

vector<Field> Stadium::getFieldByColumn(int column) {
    vector<Field> fields(rows);
    for (int i = 0; i < rows; i++) {
        fields[i] = vFields[i * columns + column];
    }
    return fields;
}
