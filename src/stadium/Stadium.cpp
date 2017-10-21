#include "Stadium.h"
#include "../util/FieldUtils.h"

Stadium::Stadium(int rows, int columns, vector<Field> vFields) :
        rows(rows), columns(columns), vFields(vFields) {}

Field *Stadium::getFieldById(int fieldId) {
    int idx = findFieldById(vFields, fieldId);
    if (idx > 0) {
        return &vFields[idx];
    }
    return nullptr;
}

int Stadium::getFieldIndex(int fieldId) {
    return findFieldById(vFields, fieldId);
}
