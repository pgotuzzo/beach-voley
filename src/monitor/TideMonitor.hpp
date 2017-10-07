#ifndef BEACH_VOLEY_MONITORMAREA_HPP
#define BEACH_VOLEY_MONITORMAREA_HPP


class TideMonitor {
private:
    enum TideChange {
        RISE_UP, FALL_DOWN, DONT_CHANGE
    };

    const int checkTideMinSeconds;
    const int checkTideMaxSeconds;
    const float riseUpTideProb;
    const float fallDownTideProb;

    TideChange simulateTide();

public:
    TideMonitor(int checkTideMaxSeconds, int checkTideMinSeconds, float fallDownTideProb,
                float riseUpTideProb);


};


#endif //BEACH_VOLEY_MONITORMAREA_HPP
