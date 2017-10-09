#ifndef BEACH_VOLEY_RANDOMNUMBER_H
#define BEACH_VOLEY_RANDOMNUMBER_H

/**
 * Creates a random integer in the interval [minValue, maxValue).
 *
 * @param minValue : the min value in the interval.
 * @param maxValue : the min value in the interval.
 * @return the random integer.
 */
int getRandomInt(int minValue, int maxValue);

/**
 * Creates a random unsigned integer in the interval [minValue, maxValue).
 *
 * @param minValue : the min value in the interval.
 * @param maxValue : the min value in the interval.
 * @return the random unsigned integer.
 */
unsigned int getRandomUnsignedInt(int minValue, int maxValue);

/**
 * Creates a random double in the interval [0, maxValue).
 *
 * @return the random double.
 */
double getRandomDouble();

#endif //BEACH_VOLEY_RANDOMNUMBER_H
