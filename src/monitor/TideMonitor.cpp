#include "TideMonitor.h"
#include "../util/RandomNumber.h"
#include "../../IPCClasses/signal/SIGINT_Handler.h"
#include "../../IPCClasses/signal/SignalHandler.h"

using namespace std;

/**
 * Logs a message with aditional information about the process and class.
 * @param message the message to log
 */
void logMessage(const string &message) {
    string messageToLog = to_string(getpid()) + string(" Tide Monitor: ") + message;
    Logger::getInstance()->logMessage(messageToLog.c_str());
    cout<<messageToLog<<endl;
}

/**
 * Tide monitor constructor initialize the attributes.
 *
 * @param checkTideMaxSeconds Maximum quantity of seconds before checking the tides.
 * @param checkTideMinSeconds Minimum quantity of seconds before checking the tides.
 * @param fallTideProb Probability that the tides fall.
 * @param riseTideProb Probability that the tides rise.
 */
TideMonitor::TideMonitor(const int checkTideMinMicroseconds, const int checkTideMaxMicroseconds, const float fallTideProb,
                         const float riseTideProb, vector<vector<int>> columnsFieldsPids)
        : checkTideMaxMicroseconds(checkTideMaxMicroseconds), checkTideMinMicroseconds(checkTideMinMicroseconds),
          fallTideProb(fallTideProb), riseTideProb(riseTideProb), columnsFieldsPids(move(columnsFieldsPids)) {
}

/**
 * Waits for a random number of seconds between the maximum and minimum. And checks if the tides change.
 *
 * @return return the status of the tides.
 */
TideMonitor::TideChange TideMonitor::simulateTide() {
    unsigned int waitMicroseconds = getRandomUnsignedInt(checkTideMinMicroseconds, checkTideMaxMicroseconds);
    logMessage(string("sleeps ") + to_string(waitMicroseconds/1000) + string(" milliseconds to check for tides"));
    usleep(waitMicroseconds);
    double p = getRandomDouble();
    if (p < riseTideProb) {
        return RISE;
    } else if (p < (riseTideProb + fallTideProb)) {
        return FALL;
    }
    return DONT_CHANGE;
}

/**
 * Start checking for tide changes an send signals to the corresponding fields
 * if the tides rise or fall. It will check till it receives a SIGINT.
 */
void TideMonitor::startMonitoring() {
    SIGINT_Handler sigint_handler;
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

    while (sigint_handler.getGracefulQuit() == 0) {
        TideChange tideChange = simulateTide();
        if (tideChange == RISE) {
            if (tideStatus + 1 < columnsFieldsPids.size() - 1) {
                tideStatus++;
                logMessage(string("the tide rises to the column ") + to_string(tideStatus));
                for (auto fieldPid: columnsFieldsPids[tideStatus]) {
                    kill(fieldPid, SIGINT);
                    logMessage(string("send notification to field pid ") + to_string(fieldPid));
                }
            }
        } else if (tideChange == FALL) {
            if (tideStatus >= 0) {
                logMessage(string("the tide falls from the column ") + to_string(tideStatus));
                for (auto fieldPid: columnsFieldsPids[tideStatus]) {
                    kill(fieldPid, SIGINT);
                    logMessage(string("send notification to field pid ") + to_string(fieldPid));
                }
                tideStatus--;
            }
        }
    }

    SignalHandler::destruir();
}