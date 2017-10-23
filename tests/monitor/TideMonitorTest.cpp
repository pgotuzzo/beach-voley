#define CATCH_CONFIG_MAIN

#include <fcntl.h>
#include "../catch.hpp"
#include "../../src/monitor/TideMonitor.h"
#include "../../IPCClasses/signal/SignalHandler.h"
#include "../../IPCClasses/signal/SIGINT_Handler.h"
#include <sys/wait.h>

using namespace std;

TEST_CASE("Integration tide monitor sends signals") {
    vector<vector<int>> columnsFieldsPids;
    int totalColumns = 4;
    int timeoutSecondsForChildProcess = 2;
    for(int i = 0; i < totalColumns; i++) {
        pid_t childPid = fork();
        if (childPid == 0) {
            SIGINT_Handler sigint_handler;
            SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);
            int timeout = 0;
            while (sigint_handler.getGracefulQuit() == 0 and timeout < timeoutSecondsForChildProcess) {
                sleep(1);
                timeout++;
            }
            SignalHandler::destruir();
            if (timeout == timeoutSecondsForChildProcess) {
                exit(1);
            }
            exit(0);
        }
        vector<int> columnFieldsPids;
        columnFieldsPids.push_back(childPid);
        columnsFieldsPids.push_back(columnFieldsPids);
    }
    pid_t tideMonitorPid = fork();
    if (tideMonitorPid == 0) {
        TideMonitor tideMonitor{20000, 300000, 0, 1, columnsFieldsPids};
        tideMonitor.startMonitoring();
        exit(0);
    }
    int status;
    for(int i = 0; i < totalColumns-1; i++) {
        wait(&status);
        REQUIRE(WEXITSTATUS(status) == 0);
    }
    // The last field doesn't receives signals.
    wait(&status);
    REQUIRE(WEXITSTATUS(status) == 1);
    // Send term to the tide monitor
    kill(tideMonitorPid, SIGTERM);
    wait(&status);
    REQUIRE(WEXITSTATUS(status) == 0);
}
