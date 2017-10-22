#ifndef BEACH_VOLEY_FIELDUTILS_H
#define BEACH_VOLEY_FIELDUTILS_H

#include "../stadium/Field.h"

int findFieldById(vector<Field> fields, int fieldId);

vector<int> findFieldsByState(vector<Field> fields, Field::State state);

#endif //BEACH_VOLEY_FIELDUTILS_H
