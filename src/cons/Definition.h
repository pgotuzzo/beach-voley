#ifndef BEACH_VOLEY_AUX_DEFINITION_H
#define BEACH_VOLEY_AUX_DEFINITION_H

#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

struct Config {
    unsigned short int rows;
    unsigned short int columns;
    unsigned short int stadiumCapacity;
    unsigned short int maxMatches;
    vector<string> vPlayerNames;
    bool debugEnabled;

    string toString() {
        string players;
        for (const string &p : vPlayerNames) {
            players += p + " ";
        }
        stringstream stream;
        stream << "Configuracion:" << endl
               << "\tPredio: " << endl
               << "\t\tTamaño: " << rows << " x " << columns << endl
               << "\t\tCapacidad: " << stadiumCapacity << endl
               << "\tJugadores: " << players << endl
               << "\tCantidad maxima de partidos por jugador: " << maxMatches << endl
               << "\tDebug habilitado? " << (debugEnabled ? "Si" : "No");
        return stream.str();
    }
};

enum TaskType {
    FIND_PARTNER, TIDE_CHANGE, MATCH_RESULT
};

struct TaskRequest {
    int pid;
    int resultLocal;
    int resultVisitant;
    bool tideRise;
    TaskType task;

    string toString() {
        string tarea;
        switch (task) {
            case FIND_PARTNER:
                tarea = "Encontrar pareja";
                break;
            case TIDE_CHANGE:
                tarea = "Marea ";
                tarea += (tideRise ? "subio" : "bajo");
                break;
            case MATCH_RESULT:
                tarea = "Actualizar resultado";
                break;
        }

        stringstream stream;
        stream << "Tarea: " << tarea << endl
               << "\tPid del solicitante: " << to_string(pid) << endl
               << "\tResultado del partido: " << endl
               << "\t\tLocal: " << to_string(resultLocal) << endl
               << "\t\tVisitante: " << to_string(resultVisitant);
        return stream.str();
    }
};

enum PlayerAction {
    PLAY, LEAVE_STADIUM, LEAVE_TOURNAMENT
};

struct OrgPlayerResponse {
    int fieldId;
    PlayerAction playerAction;

    string toString() {
        string action;
        switch (playerAction) {
            case PLAY:
                action = "JUGAR";
                break;
            case LEAVE_STADIUM:
                action = "ABANDONAR EL PREDIO";
                break;
            case LEAVE_TOURNAMENT:
                action = "ABANDONAR EL TORNEO";
                break;
        }

        stringstream message;
        message << "Cancha: (PID: " << fieldId << ") " << endl
                << "Accion: " << action;
        return message.str();
    }

};

#endif //BEACH_VOLEY_AUX_DEFINITION_H
