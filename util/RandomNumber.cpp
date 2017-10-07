#include <random>
#include "RandomNumber.hpp"

int getRandomInt(const int minValue, const int maxValue) {
    srand(std::random_device()());
    return minValue + (rand() % (maxValue - minValue));
}

double getRandomDouble() {
    srand(std::random_device()());
    return drand48();
}