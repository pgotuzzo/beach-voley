#include "../catch.hpp"
#include "../../src/player/Player.h"
#include "../../src/managers/Manager.h"
#include "../../src/config/Constants.h"
#include "../../src/util/ResourceHandler.h"
#include <sys/wait.h>

using namespace std;

TEST_CASE("Check one game 4 players tournament") {
    unsigned int totalPlayers = 4;
    int fieldId = 0;
    unsigned int totalFields = 1;
    unsigned int totalMatchs = 1;
    VectorCompartido<int> pointsTable;
    pointsTable.crear("/bin/rm", 'a', totalPlayers);
    LockFile lockForSharedVectors(LOCK_FILE_SHARED_VECTORS);

    Pipe taskToManager;
    Pipe receiveResponse[4];
    vector<Pipe *> playersIdPipeVector;
    for (int i = 0; i < totalPlayers; i++) {
        playersIdPipeVector.push_back(&receiveResponse[i]);
    }

    __pid_t managerPid = fork();
    if (managerPid == 0) {
        Manager manager{totalFields, totalPlayers, totalMatchs, totalPlayers, &lockForSharedVectors,
                        &pointsTable, &taskToManager, receiveResponse, totalPlayers};
        manager.receiveTask();
        exit(0);
    }

    for(int i = 0; i < totalPlayers; i++) {
        __pid_t playerPid = fork();
        if (playerPid == 0) {
            taskToManager.setearModo(Pipe::ESCRITURA);
            receiveResponse[i].setearModo(Pipe::LECTURA);

            TaskRequest taskRequest = {i, 0, 0, false, FIND_PARTNER};
            taskToManager.escribir(&taskRequest, sizeof(TaskRequest));

            OrgPlayerResponse response{};
            ssize_t out = receiveResponse[i].leer((&response), sizeof(OrgPlayerResponse));
            REQUIRE(out == sizeof(OrgPlayerResponse));
            REQUIRE(response.playerAction == ENUM_PLAY);
            REQUIRE(response.fieldId == fieldId);
            sleep(3);

            taskToManager.escribir(&taskRequest, sizeof(TaskRequest));
            out = receiveResponse[i].leer((&response), sizeof(OrgPlayerResponse));
            REQUIRE(out == sizeof(OrgPlayerResponse));
            REQUIRE(response.playerAction == ENUM_LEAVE_TOURNAMENT);

            exit(0);
        }
    }

    sleep(1);
    taskToManager.setearModo(Pipe::ESCRITURA);
    TaskRequest taskRequest = {fieldId, 3, 1, false, MATCH_RESULT};
    taskToManager.escribir(&taskRequest, sizeof(TaskRequest));

    int status;
    for(int i = 0; i < totalPlayers + 1; i++) {
        wait(&status);
        REQUIRE(WEXITSTATUS(status) == 0);
    }
}