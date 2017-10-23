#include <fcntl.h>
#include "../catch.hpp"
#include "../../src/monitor/TideMonitor.h"
#include "../../IPCClasses/signal/SignalHandler.h"
#include "../../IPCClasses/signal/SIGINT_Handler.h"
#include "../../src/stadium/Field.h"
#include <sys/wait.h>

using namespace std;

TEST_CASE("Check play one game") {
    unsigned short fieldId = 10;
    Semaforo entranceSemaphore = Semaforo("/bin/ls", 0, 30);
    Semaforo exitSemaphore = Semaforo("/bin/sleep", 0, 30);
    Pipe taskToManager;
    int totalPlayers = 4;
    for(int i = 0; i < totalPlayers; i++) {
        pid_t childPid = fork();
        if (childPid == 0) {
            entranceSemaphore.v(fieldId);
            exitSemaphore.p(fieldId);
            exit(0);
        }
    }
    pid_t fieldPid = fork();
    if (fieldPid == 0) {
        Field field{fieldId, &entranceSemaphore, &exitSemaphore, &taskToManager, 100000, 300000};
        field.startPlayingGames();
        exit(0);
    }

    // check result to manager
    TaskRequest task{};
    ssize_t out = taskToManager.leer(static_cast<void *>(&task), sizeof(TaskRequest));
    REQUIRE(out == sizeof(TaskRequest));
    REQUIRE(task.id == fieldId);
    REQUIRE(task.task == MATCH_RESULT);
    // check it release the players
    int status;
    for(int i = 0; i < totalPlayers; i++) {
        wait(&status);
        REQUIRE(WEXITSTATUS(status) == 0);
    }
    // Send kill signal to the field
    kill(fieldPid, SIGKILL);
    wait(&status);
    REQUIRE(WEXITSTATUS(status) == 0);
    entranceSemaphore.eliminar();
    exitSemaphore.eliminar();
    taskToManager.cerrar();
}

TEST_CASE("Check interrupt before game") {
    unsigned short fieldId = 10;
    Semaforo entranceSemaphore = Semaforo("/bin/ls", 0, 30);
    Semaforo exitSemaphore = Semaforo("/bin/sleep", 0, 30);
    Pipe taskToManager;
    pid_t fieldPid = fork();
    if (fieldPid == 0) {
        Field field{fieldId, &entranceSemaphore, &exitSemaphore, &taskToManager, 100000, 300000};
        field.startPlayingGames();
        exit(0);
    }
    taskToManager.setearModo(Pipe::LECTURA);
    sleep(4);
    // sends flood signal
    kill(fieldPid, SIGINT);

    // check notification to manager
    TaskRequest task{};
    ssize_t out = taskToManager.leer(static_cast<void *>(&task), sizeof(TaskRequest));
    REQUIRE(out == sizeof(TaskRequest));
    REQUIRE(task.id == fieldId);
    REQUIRE(task.task == TIDE_CHANGE);
    REQUIRE(task.tideRise);
    int status;
    // Send term to the field
    kill(fieldPid, SIGKILL);
    wait(&status);
    REQUIRE(WEXITSTATUS(status) == 0);
    entranceSemaphore.eliminar();
    exitSemaphore.eliminar();
    taskToManager.cerrar();
}

TEST_CASE("Check interrupt during game") {
    unsigned short fieldId = 10;
    Semaforo entranceSemaphore = Semaforo("/bin/ls", 0, 30);
    Semaforo exitSemaphore = Semaforo("/bin/sleep", 0, 30);
    Pipe taskToManager;
    int totalPlayers = 4;
    for(int i = 0; i < totalPlayers; i++) {
        pid_t childPid = fork();
        if (childPid == 0) {
            entranceSemaphore.v(fieldId);
            exitSemaphore.p(fieldId);
            exit(0);
        }
    }
    pid_t fieldPid = fork();
    if (fieldPid == 0) {
        Field field{fieldId, &entranceSemaphore, &exitSemaphore, &taskToManager, 10000000, 30000000};
        field.startPlayingGames();
        exit(0);
    }
    taskToManager.setearModo(Pipe::LECTURA);
    sleep(4);
    // sends flood signal
    kill(fieldPid, SIGINT);

    // check notification to manager
    TaskRequest task{};
    ssize_t out = taskToManager.leer(static_cast<void *>(&task), sizeof(TaskRequest));
    REQUIRE(out == sizeof(TaskRequest));
    REQUIRE(task.id == fieldId);
    REQUIRE(task.task == TIDE_CHANGE);
    REQUIRE(task.tideRise);
    // check it release the players
    int status;
    for(int i = 0; i < totalPlayers; i++) {
        wait(&status);
        REQUIRE(WEXITSTATUS(status) == 0);
    }
    // Send term to the field
    kill(fieldPid, SIGKILL);
    wait(&status);
    REQUIRE(WEXITSTATUS(status) == 0);
    entranceSemaphore.eliminar();
    exitSemaphore.eliminar();
    taskToManager.cerrar();
}
