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
#include "ipc/signal/SignalHandler.h"
#include "ipc/signal/SIGINT_Handler.h"
#include "stadium/FieldProcess.h"

int main() {
    // Remove any previous log
    Logger::reset();

    // Read parameters
    // FIXME - Reuse previous code
    Logger::d("Se leen los parametros...");
    Logger::d("Configuracion valida!");

    Config config;
    config.rows = 2;
    config.columns = 2;
    config.maxMatches = 5;
    config.stadiumCapacity = 9;
    config.vPlayerNames = {"Pablo", "Dani", "Marta", "Pipo", "Lola", "Martin", "Gonzo", "Carla", "Charlie", "Morena",
                           "Toto", "Jazmin"};
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
    Semaforo *fieldEntrance = ResourceHandler::getSemaforo(PATH_SEM_FIELD_ENTRANCE);
    Semaforo *fieldExit = ResourceHandler::getSemaforo(PATH_SEM_FIELD_EXIT);

    // Create Launcher Process
    Pipe *managerQueue = ResourceHandler::getPipe(PIPE_ID_MANAGER_QUEUE);
    auto playersAmount = static_cast<unsigned short int>(config.vPlayerNames.size());
    LauncherProcess tournamentLauncher(playersAmount, tournamentSubscription, tournamentStart);
    int tournamentPid = tournamentLauncher.start();
    Logger::d("Proceso - Recepcion: (PID) " + to_string(tournamentPid));

    // Create Stadium - Field Processes
    vector<int> vFieldsPid;
    vector<Field> vFields(config.rows * config.columns);
    for (int i = 0; i < config.rows; i++) {
        for (int j = 0; j < config.columns; j++) {
            string fieldName = to_string(i) + to_string(j);
            int fieldIdx = i * config.columns + j;

            FieldProcess fieldProcess(fieldName, managerQueue, fieldIdx, fieldEntrance, fieldExit);
            int fieldId = fieldProcess.start();
            vFieldsPid.push_back(fieldId);
            Logger::d("Proceso - Cancha: (PID) " + to_string(fieldId));

            Field field(fieldId, fieldIdx, fieldName);
            vFields[fieldIdx] = field;
        }
    }
    Stadium stadium(config.rows, config.columns, vFields);

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
                                    fieldEntrance,
                                    fieldExit,
                                    managerQueue,
                                    responseToPlayer,
                                    &stadium);
        int playerPid = playerProcess.start();
        Logger::d("Proceso - Jugador " + config.vPlayerNames[i] + ": (PID) " + to_string(playerPid));
        Player player(playerPid, playerName, responseToPlayer);
        vPlayers.push_back(player);
        vPlayersPid.push_back(playerPid);
    }

    // Create Manager Process
    ManagerProcess managerProcess = ManagerProcess(&vPlayers, managerQueue, &stadium, config.maxMatches,
                                                   config.stadiumCapacity);
    int managerPid = managerProcess.start();
    Logger::d("Proceso - Organizador: (PID) " + to_string(managerPid));

    // Create Results Table
    // TODO - Implement!

    // Create Tide Monitor
    // TODO - Implement

    //      Signal handler in order to avoid ipc resources being wasted
    SIGINT_Handler handler;
    SignalHandler::getInstance()->registrarHandler(SIGINT, (EventHandler *) &handler);

    // Wait for children processes
    for (int i = 0; i < (vPlayersPid.size() + 2); i++) {
        int res;
        int pid = wait(&res);
        string result = res == 0 ? "EXITOSAMENTE" : "MAL (Error: " + to_string(res) + ")";
        auto it = find(vPlayersPid.begin(), vPlayersPid.end(), pid);
        if (it != vPlayersPid.end()) {
            Logger::d("Proceso - Jugador: (PID) " + to_string(pid) + " FINALIZO " + result);
        } else if (pid == managerPid) {
            Logger::d("Proceso - Organizador: (PID) " + to_string(pid) + " FINALIZO " + result);
        } else if (pid == tournamentPid) {
            Logger::d("Proceso - Recepcion: (PID) " + to_string(pid) + " FINALIZO " + result);
        } else if (find(vFieldsPid.begin(), vFieldsPid.end(), pid) != vFieldsPid.end()) {
            Logger::e("Proceso - Cancha: (PID) " + to_string(pid) + " FINALIZO " + result);
            Logger::e(
                    "Los procesos asigandos a cada cancha NO deben finalizar a menos que el toreo haya terminado. Algo salio (MUY) mal!");
        }
    }

    for (int pid : vFieldsPid) {
        Logger::d("MATANDO al Proceso - Cancha: (PID) " + to_string(pid));
        kill(pid, SIGKILL);
    }

    // Free Resources
    ResourceHandler::freeResources();

    Logger::i("===========================================================================================");
    Logger::i("============                    BEACH VOLEY - FINAL                             ===========");
    Logger::i("===========================================================================================");
    return 0;
}