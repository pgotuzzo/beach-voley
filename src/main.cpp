#include <iostream>
#include "util/Logger.h"
#include "cons/Definition.h"
#include "ipc/Semaforo.h"
#include "cons/Path.h"
#include "player/Player.h"
#include "LauncherProcess.h"
#include "manager/ManagerProcess.h"
#include "util/ResourceHandler.h"
#include "cons/Constants.h"
#include "player/PlayerProcess.h"

int main() {
    // Remove any previous log
    Logger::reset();

    // Read parameters
    Logger::d("Se leen los parametros...");
    Logger::d("Configuracion valida!");

    Config config;
    config.rows = 2;
    config.columns = 2;
    config.maxMatches = 3;
    config.stadiumCapacity = 5;
    config.vPlayerNames = {"Pablo", "Marta", "Pipo", "Lola", "Martin", "Gonzo", "Carla", "Charlie", "Morena", "Toto",
                           "Jazmin"};
    config.debugEnabled = true;

    Logger::d(config.toString());

    Logger::i("===========================================================================================");
    Logger::i("============                    BEACH VOLEY - COMIENZO                          ===========");
    Logger::i("===========================================================================================");

    // Resource Handler initialization - Creates all the IPC objects
    ResourceHandler::init(config);

    // Semaforos
    Semaforo *tournamentSubscription = ResourceHandler::getSemaforo(PATH_SEM_TOURNAMENT_SUBSCRIPTION);
    Semaforo *tournamentStart = ResourceHandler::getSemaforo(PATH_SEM_TOURNAMENT_START);
    Semaforo *stadiumTurnstile = ResourceHandler::getSemaforo(PATH_SEM_TURNSTILE);

    // Create Launcher Process
    Pipe *managerQueue = ResourceHandler::getPipe(PIPE_ID_MANAGER_QUEUE);
    auto playersAmount = static_cast<unsigned short int>(config.vPlayerNames.size());
    LauncherProcess tournamentLauncher(playersAmount, tournamentSubscription, tournamentStart);
    int tournamentPid = tournamentLauncher.start();
    Logger::d("Proceso - Launcher: (PID) " + to_string(tournamentPid));

    // Create Players Processes
    vector<Player> vPlayers;
    vector<int> vPlayersPid;
    for (int i = 0; i < config.vPlayerNames.size(); i++) {
        string playerName = config.vPlayerNames[i];

        // Pipe - Response to player
        Pipe *responseToPlayer = ResourceHandler::getPipe(PIPE_ID_RESPONSE_TO_PLYER_N + playerName);

        PlayerProcess playerProcess(playerName,
                                    tournamentSubscription,
                                    tournamentStart,
                                    stadiumTurnstile,
                                    managerQueue,
                                    responseToPlayer);
        int playerPid = playerProcess.start();
        Logger::d("Proceso - Jugador " + config.vPlayerNames[i] + ": (PID) " + to_string(playerPid));
        Player player(playerPid, playerName, responseToPlayer);
        vPlayers.push_back(player);
        vPlayersPid.push_back(playerPid);
    }

    // Create Manager Process
    ManagerProcess managerProcess = ManagerProcess(&vPlayers, managerQueue, config.maxMatches, config.stadiumCapacity);
    int managerPid = managerProcess.start();
    Logger::d("Proceso - Manager: (PID) " + to_string(managerPid));

    // Create Stadium

    // Create Results Table

    // Wait for children processes
    for (int i = 0; i < (vPlayersPid.size() + 3); i++) {
        int res;
        int pid = wait(&res);
        string result = res == 0 ? "EXITOSAMENTE" : "MAL (Error: " + to_string(res) + ")";
        auto it = find(vPlayersPid.begin(), vPlayersPid.end(), pid);
        if (it != vPlayersPid.end()) {
            Logger::d("Proceso - Jugador: (PID) " + to_string(pid) + " FINALIZO " + result);
        } else if (pid == managerPid) {
            Logger::d("Proceso - Organizador: (PID) " + to_string(pid) + " FINALIZO " + result);
        } else if (pid == tournamentPid) {
            Logger::d("Proceso - Launcher: (PID) " + to_string(pid) + " FINALIZO " + result);
        }
    }

    // Free Resources
    ResourceHandler::freeResources();

    Logger::i("===========================================================================================");
    Logger::i("============                    BEACH VOLEY - FINAL                             ===========");
    Logger::i("===========================================================================================");
    return 0;
}