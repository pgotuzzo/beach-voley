#ifndef BEACH_VOLEY_MONITORMAREA_H
#define BEACH_VOLEY_MONITORMAREA_H


class TideMonitor {
private:
    enum TideChange {
        RISE, FALL, DONT_CHANGE
    };

    const int checkTideMinSeconds;
    const int checkTideMaxSeconds;
    const float riseTideProb;
    const float fallTideProb;

    TideChange simulateTide();

public:
    TideMonitor(int checkTideMaxSeconds, int checkTideMinSeconds, float fallTideProb,
                float riseTideProb);


};


#endif //BEACH_VOLEY_MONITORMAREA_H
