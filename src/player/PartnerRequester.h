#ifndef BEACH_VOLEY_PARTNERREQUESTER_H
#define BEACH_VOLEY_PARTNERREQUESTER_H

#include <string>
#include "../config/Definitions.h"
#include "../../IPCClasses/LockFile.h"
#include "../../IPCClasses/Pipe.h"

using namespace std;

class PartnerRequester {
private:
    int playerId;
    string playerName;
    Pipe *receiveResponsesPipe;
    Pipe *sendRequestPipe;

public:
    explicit PartnerRequester(int playerId, const string &playerName, Pipe *receiveResponsesPipe,
                              Pipe *sendRequestPipe);

    void request();

    OrgPlayerResponse waitResponse();
};


#endif //BEACH_VOLEY_PARTNERREQUESTER_H
