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