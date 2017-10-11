#ifndef BEACH_VOLEY_PARTNERREQUESTER_H
#define BEACH_VOLEY_PARTNERREQUESTER_H

#include <string>
#include "../config/Definitions.h"
#include "../../IPCClasses/fifo/FifoRead.h"
#include "../../IPCClasses/fifo/FifoWrite.h"
#include "../../IPCClasses/LockFile.h"

using namespace std;

class PartnerRequester {
private:
    int playerId;
    string playerName;
    FifoRead *fifoRead;
    FifoWrite *fifoWrite;

public:
    explicit PartnerRequester(int playerId, const string &playerName);

    void request();

    OrgPlayerResponse waitResponse();
};


#endif //BEACH_VOLEY_PARTNERREQUESTER_H
