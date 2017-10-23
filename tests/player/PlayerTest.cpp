#include "../catch.hpp"
#include "../../src/player/Player.h"
#include <sys/wait.h>

using namespace std;

TEST_CASE("Check player leaves the tournament") {
    int playerId = 7;
    int stadiumSize = 10;
    Semaforo entranceSemaphore = Semaforo("/bin/ls", 0, 30);
    Semaforo exitSemaphore = Semaforo("/bin/sleep", 0, 30);
    Semaforo stadiumTurnstile = Semaforo("/bin/rm", 10, 1);

    Pipe taskToManager;
    Pipe receiveResponse;

    pid_t fieldPid = fork();
    if (fieldPid == 0) {
        Player player{playerId, &entranceSemaphore, &exitSemaphore, &stadiumTurnstile,
                      &receiveResponse, &taskToManager};
        player.play();
        exit(0);
    }
    taskToManager.setearModo(Pipe::LECTURA);
    receiveResponse.setearModo(Pipe::ESCRITURA);
    sleep(1);
    // Check the player is inside the stadium
    REQUIRE(stadiumSize - 1 == stadiumTurnstile.getStatus(0));

    // check result to manager
    TaskRequest task{};
    ssize_t out = taskToManager.leer(static_cast<void *>(&task), sizeof(TaskRequest));
    REQUIRE(out == sizeof(TaskRequest));
    REQUIRE(task.id == playerId);
    REQUIRE(task.task == FIND_PARTNER);

    OrgPlayerResponse orgPlayerResponse{0, ENUM_LEAVE_TOURNAMENT};
    receiveResponse.escribir(static_cast<const void *> (&orgPlayerResponse), sizeof(OrgPlayerResponse));

    int status;
    wait(&status);
    REQUIRE(WEXITSTATUS(status) == 0);
    REQUIRE(stadiumSize == stadiumTurnstile.getStatus(0));
}

TEST_CASE("Check player plays one game") {
    int fieldId = 8;
    int playerId = 7;
    int stadiumSize = 10;
    Semaforo entranceSemaphore = Semaforo("/bin/ls", 0, 30);
    Semaforo exitSemaphore = Semaforo("/bin/sleep", 0, 30);
    Semaforo stadiumTurnstile = Semaforo("/bin/rm", 10, 1);

    Pipe taskToManager;
    Pipe receiveResponse;

    pid_t fieldPid = fork();
    if (fieldPid == 0) {
        Player player{playerId, &entranceSemaphore, &exitSemaphore, &stadiumTurnstile,
                      &receiveResponse, &taskToManager};
        player.play();
        exit(0);
    }
    taskToManager.setearModo(Pipe::LECTURA);
    receiveResponse.setearModo(Pipe::ESCRITURA);
    sleep(1);
    // Check the player is inside the stadium
    REQUIRE(stadiumSize - 1 == stadiumTurnstile.getStatus(0));

    // check result to manager
    TaskRequest task{};
    ssize_t out = taskToManager.leer(static_cast<void *>(&task), sizeof(TaskRequest));
    REQUIRE(out == sizeof(TaskRequest));
    REQUIRE(task.id == playerId);
    REQUIRE(task.task == FIND_PARTNER);

    OrgPlayerResponse orgPlayerResponse{fieldId, ENUM_PLAY};
    receiveResponse.escribir(static_cast<const void *> (&orgPlayerResponse), sizeof(OrgPlayerResponse));

    entranceSemaphore.p(fieldId);
    exitSemaphore.v(fieldId);

    // check result to manager
    out = taskToManager.leer(static_cast<void *>(&task), sizeof(TaskRequest));
    REQUIRE(out == sizeof(TaskRequest));
    REQUIRE(task.id == playerId);
    REQUIRE(task.task == FIND_PARTNER);

    orgPlayerResponse.playerAction = ENUM_LEAVE_TOURNAMENT;
    receiveResponse.escribir(static_cast<const void *> (&orgPlayerResponse), sizeof(OrgPlayerResponse));

    int status;
    wait(&status);
    REQUIRE(WEXITSTATUS(status) == 0);
    REQUIRE(stadiumSize == stadiumTurnstile.getStatus(0));
}

TEST_CASE("Check player leaves the stadium reenters and plays one game") {
    int fieldId = 8;
    int playerId = 7;
    int stadiumSize = 10;
    Semaforo entranceSemaphore = Semaforo("/bin/ls", 0, 30);
    Semaforo exitSemaphore = Semaforo("/bin/sleep", 0, 30);
    Semaforo stadiumTurnstile = Semaforo("/bin/rm", 10, 1);

    Pipe taskToManager;
    Pipe receiveResponse;

    pid_t fieldPid = fork();
    if (fieldPid == 0) {
        Player player{playerId, &entranceSemaphore, &exitSemaphore, &stadiumTurnstile,
                      &receiveResponse, &taskToManager};
        player.play();
        exit(0);
    }
    taskToManager.setearModo(Pipe::LECTURA);
    receiveResponse.setearModo(Pipe::ESCRITURA);
    sleep(1);
    // Check the player is inside the stadium
    REQUIRE(stadiumSize - 1 == stadiumTurnstile.getStatus(0));

    // check result to manager
    TaskRequest task{};
    ssize_t out = taskToManager.leer(static_cast<void *>(&task), sizeof(TaskRequest));
    REQUIRE(out == sizeof(TaskRequest));
    REQUIRE(task.id == playerId);
    REQUIRE(task.task == FIND_PARTNER);

    OrgPlayerResponse orgPlayerResponse{fieldId, ENUM_LEAVE_STADIUM};
    receiveResponse.escribir(static_cast<const void *> (&orgPlayerResponse), sizeof(OrgPlayerResponse));

    // check result to manager
    out = taskToManager.leer(static_cast<void *>(&task), sizeof(TaskRequest));
    REQUIRE(out == sizeof(TaskRequest));
    REQUIRE(task.id == playerId);
    REQUIRE(task.task == FIND_PARTNER);

    orgPlayerResponse.playerAction = ENUM_PLAY;
    receiveResponse.escribir(static_cast<const void *> (&orgPlayerResponse), sizeof(OrgPlayerResponse));

    entranceSemaphore.p(fieldId);
    exitSemaphore.v(fieldId);

    // check result to manager
    out = taskToManager.leer(static_cast<void *>(&task), sizeof(TaskRequest));
    REQUIRE(out == sizeof(TaskRequest));
    REQUIRE(task.id == playerId);
    REQUIRE(task.task == FIND_PARTNER);

    orgPlayerResponse.playerAction = ENUM_LEAVE_TOURNAMENT;
    receiveResponse.escribir(static_cast<const void *> (&orgPlayerResponse), sizeof(OrgPlayerResponse));

    int status;
    wait(&status);
    REQUIRE(WEXITSTATUS(status) == 0);
    REQUIRE(stadiumSize == stadiumTurnstile.getStatus(0));
}
