#ifndef BEACH_VOLEY_TIDEMONITOR_H
#define BEACH_VOLEY_TIDEMONITOR_H

#include <vector>
#include "../logger/Logger.h"

using namespace std;

class TideMonitor {
private:
    enum TideChange {
        RISE, FALL, DONT_CHANGE
    };

    const int checkTideMinMicroseconds;
    const int checkTideMaxMicroseconds;
    const float riseTideProb;
    const float fallTideProb;
    // Every vector inside the main vector is a column with field pids.
    vector<vector<int>> columnsFieldsPids;
    // This represents in what column is the tide. (-1 is in the sea).
    int tideStatus = -1;

    TideChange simulateTide();

public:
    TideMonitor(int checkTideMinMicroseconds, int checkTideMaxMicroseconds, float fallTideProb,
                float riseTideProb, vector<vector<int>> columnsFieldsPids);

    void startMonitoring();
};


#endif //BEACH_VOLEY_TIDEMONITOR_H
