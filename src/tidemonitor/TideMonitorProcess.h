#ifndef BEACH_VOLEY_TIDEMONITORPROCESS_H
#define BEACH_VOLEY_TIDEMONITORPROCESS_H

#include <vector>
#include "../stadium/Stadium.h"

using namespace std;

class TideMonitorProcess {
private:
    enum TideChange {
        RISE, FALL, DONT_CHANGE
    };

    const int checkTideMinMicroseconds;
    const int checkTideMaxMicroseconds;
    const float riseTideProb;
    const float fallTideProb;
    Stadium *stadium;

    // This represents in what column is the tide. (-1 is in the sea).
    int tideStatus = -1;

    TideChange simulateTide();

    void startMonitoring();

public:
    TideMonitorProcess(int checkTideMinMicroseconds, int checkTideMaxMicroseconds, float fallTideProb,
                float riseTideProb, Stadium *stadium);

    int start();

};


#endif //BEACH_VOLEY_TIDEMONITORPROCESS_H
