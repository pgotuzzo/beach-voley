#ifndef BEACH_VOLEY_MONITORMAREA_H
#define BEACH_VOLEY_MONITORMAREA_H


#include <vector>

using namespace std;

class TideMonitor {
private:
    enum TideChange {
        RISE, FALL, DONT_CHANGE
    };

    const int checkTideMinSeconds;
    const int checkTideMaxSeconds;
    unsigned long totalColumns;
    const float riseTideProb;
    const float fallTideProb;
    vector<vector<int>> columnFieldsPids;
    int tideStatus = -1;

    TideChange simulateTide();

public:
    TideMonitor(int checkTideMaxSeconds, int checkTideMinSeconds, float fallTideProb,
                float riseTideProb, vector<vector<int>> columnFieldsPids);

    int startMonitoring();
};


#endif //BEACH_VOLEY_MONITORMAREA_H
