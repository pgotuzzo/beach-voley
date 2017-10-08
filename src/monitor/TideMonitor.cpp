#include <unistd.h>
#include "TideMonitor.hpp"
#include "../../util/RandomNumber.hpp"

/**
 * Tide monitor constructor initialize the attributes.
 *
 * @param checkTideMaxSeconds Maximum quantity of seconds before checking the tides.
 * @param checkTideMinSeconds Minimum quantity of seconds before checking the tides.
 * @param fallDownTideProb Probability that the tides fall down.
 * @param riseUpTideProb Probability that the tides rise up.
 */
TideMonitor::TideMonitor(const int checkTideMaxSeconds, const int checkTideMinSeconds, const float fallDownTideProb,
                         const float riseUpTideProb)
        : checkTideMaxSeconds(checkTideMaxSeconds), checkTideMinSeconds(checkTideMinSeconds),
          fallDownTideProb(fallDownTideProb),
          riseUpTideProb(riseUpTideProb) {
}

/**
 * Waits for a random number of seconds between the maximum and minimum. And checks if the tides change.
 *
 * @return return the status of the tides.
 */
TideMonitor::TideChange TideMonitor::simulateTide() {
    sleep(getRandomUnsignedInt(checkTideMinSeconds, checkTideMaxSeconds));
    double p = getRandomDouble();
    if (p < riseUpTideProb) {
        return RISE_UP;
    } else if (p < (riseUpTideProb + fallDownTideProb)) {
        return FALL_DOWN;
    }
    return DONT_CHANGE;
}
