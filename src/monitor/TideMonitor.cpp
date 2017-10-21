#include <unistd.h>
#include <signal.h>

#include <utility>
#include <iostream>
#include "TideMonitor.h"
#include "../../util/RandomNumber.h"

using namespace std;

/**
 * Tide monitor constructor initialize the attributes.
 *
 * @param checkTideMaxSeconds Maximum quantity of seconds before checking the tides.
 * @param checkTideMinSeconds Minimum quantity of seconds before checking the tides.
 * @param fallTideProb Probability that the tides fall.
 * @param riseTideProb Probability that the tides rise.
 */
TideMonitor::TideMonitor(const int checkTideMaxSeconds, const int checkTideMinSeconds, const float fallTideProb,
                         const float riseTideProb, vector<vector<int>> columnFieldsPids)
        : checkTideMaxSeconds(checkTideMaxSeconds), checkTideMinSeconds(checkTideMinSeconds),
          fallTideProb(fallTideProb), riseTideProb(riseTideProb), columnFieldsPids(move(columnFieldsPids)),
          totalColumns(columnFieldsPids.size()) {
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
        cout << "Tide rise"<< endl;
        return RISE;
    } else if (p < (riseTideProb + fallTideProb)) {
        cout << "Tide fall"<< endl;
        return FALL;
    }
    return DONT_CHANGE;
}

/**
 * Start checking for tide changes an send signals to the fields
 * if the tides rise or fall.
 */
int TideMonitor::startMonitoring() {
    __pid_t pid = fork();
    if (pid == 0) {
        cout<< getpid()<< " tide"<<endl;
        bool tournamentEnded = false;
        while (!tournamentEnded) {
            TideChange tideChange = simulateTide();
            if (tideChange == RISE) {
                if (tideStatus + 1 < totalColumns - 1) {
                    tideStatus++;
                    for (auto fieldPid: columnFieldsPids[tideStatus]) {
                        kill(fieldPid, SIGINT);
                    }
                }
            } else if (tideChange == FALL) {
                if (tideStatus >= 0) {
                    for (auto fieldPid: columnFieldsPids[tideStatus]) {
                        kill(fieldPid, SIGINT);
                    }
                    tideStatus--;
                }
            }
        }
    }
    return pid;
}