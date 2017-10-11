#include <unistd.h>
#include "TideMonitor.h"
#include "../../util/RandomNumber.h"

/**
 * Tide monitor constructor initialize the attributes.
 *
 * @param checkTideMaxSeconds Maximum quantity of seconds before checking the tides.
 * @param checkTideMinSeconds Minimum quantity of seconds before checking the tides.
 * @param fallTideProb Probability that the tides fall.
 * @param riseTideProb Probability that the tides rise.
 */
TideMonitor::TideMonitor(const int checkTideMaxSeconds, const int checkTideMinSeconds, const float fallTideProb,
                         const float riseTideProb)
        : checkTideMaxSeconds(checkTideMaxSeconds), checkTideMinSeconds(checkTideMinSeconds),
          fallTideProb(fallTideProb),
          riseTideProb(riseTideProb) {
}

/**
 * Waits for a random number of seconds between the maximum and minimum. And checks if the tides change.
 *
 * @return return the status of the tides.
 */
TideMonitor::TideChange TideMonitor::simulateTide() {
    sleep(getRandomUnsignedInt(checkTideMinSeconds, checkTideMaxSeconds));
    double p = getRandomDouble();
    if (p < riseTideProb) {
        return RISE;
    } else if (p < (riseTideProb + fallTideProb)) {
        return FALL;
    }
    return DONT_CHANGE;
}
