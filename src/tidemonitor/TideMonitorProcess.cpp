#include <string>
#include <zconf.h>
#include "TideMonitorProcess.h"
#include "../util/RandomNumber.h"
#include "../ipc/signal/SignalHandler.h"
#include "../ipc/signal/SIGTERM_Handler.h"
#include "../util/Logger.h"

using namespace std;

/**
 * Logs a message with aditional information about the process and class.
 * @param message the message to log
 */
void logMessage(const string &message) {
    string messageToLog = string(" Tide Monitor: ") + message;
    Logger::d(messageToLog.c_str());
}

/**
 * Tide monitor constructor initialize the attributes.
 *
 * @param checkTideMinMicroseconds Maximum quantity of microseconds before checking the tides.
 * @param checkTideMaxMicroseconds Minimum quantity of microseconds before checking the tides.
 * @param fallTideProb Probability that the tides fall.
 * @param riseTideProb Probability that the tides rise.
 */
TideMonitorProcess::TideMonitorProcess(const int checkTideMinMicroseconds, const int checkTideMaxMicroseconds,
                                       const float fallTideProb,
                                       const float riseTideProb, Stadium *stadium)
        : checkTideMaxMicroseconds(checkTideMaxMicroseconds), checkTideMinMicroseconds(checkTideMinMicroseconds),
          fallTideProb(fallTideProb), riseTideProb(riseTideProb), stadium(stadium) {}

/**
 * Waits for a random number of seconds between the maximum and minimum. And checks if the tides change.
 *
 * @return return the status of the tides.
 */
TideMonitorProcess::TideChange TideMonitorProcess::simulateTide() {
    unsigned int waitMicroseconds = getRandomUnsignedInt(checkTideMinMicroseconds, checkTideMaxMicroseconds);
    logMessage(string("durmiendo ") + to_string(waitMicroseconds / 1000) +
               string(" milisegundos antes de volver a chequear la marea"));
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
void TideMonitorProcess::startMonitoring() {
    SIGTERM_Handler sigterm_handler;
    SignalHandler::getInstance()->registrarHandler(SIGTERM, &sigterm_handler);

    while (sigterm_handler.getGracefulQuit() == 0) {
        TideChange tideChange = simulateTide();
        if (tideChange == RISE) {
            if (tideStatus + 1 < stadium->getColumnsCount() - 1) {
                tideStatus++;
                logMessage(string("la marea subio hasta la columna: ") + to_string(tideStatus));
                vector<Field> column = stadium->getFieldByColumn(tideStatus);
                for (auto field : column) {
                    kill(field.getId(), SIGINT);
                    logMessage(string("Notificando a la Cancha ") + field.getName());
                }
            }
        } else if (tideChange == FALL) {
            if (tideStatus >= 0) {
                logMessage(string("la marea bajo de la columna ") + to_string(tideStatus));
                vector<Field> column = stadium->getFieldByColumn(tideStatus);
                for (auto field: column) {
                    kill(field.getId(), SIGINT);
                    logMessage(string("Notificantdo a la Cancha ") + field.getName());
                }
                tideStatus--;
            }
        }
    }

    SignalHandler::destruir();
}

int TideMonitorProcess::start() {
    int pid = fork();
    if (pid == 0) {
        startMonitoring();
        exit(0);
    }
    return pid;
}
