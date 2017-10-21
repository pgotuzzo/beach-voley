#include <random>
#include "RandomNumber.h"

int getRandomInt(const int minValue, const int maxValue) {
    srand(std::random_device()());
    return minValue + rand() % (maxValue - minValue);
}

unsigned int getRandomUnsignedInt(const int minValue, const int maxValue) {
    return static_cast<unsigned int>(getRandomInt(minValue, maxValue));
}

double getRandomDouble() {
    srand(std::random_device()());
    return drand48();
}

bool getRandomBool() {
    int res = getRandomInt(0, 1);
    return res == 0;
}