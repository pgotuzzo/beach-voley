#ifndef BEACH_VOLEY_PARTNERREQUESTER_H
#define BEACH_VOLEY_PARTNERREQUESTER_H

#include <string>
#include "../Definitions.h"
#include "../../IPCClasses/fifo/FifoRead.h"
#include "../../IPCClasses/fifo/FifoWrite.h"

using namespace std;

class PartnerRequester {
private:
    string playerName;
    FifoRead *fifoRead;
    FifoWrite *fifoWrite;

public:
    PartnerRequester(const string &playerName);

    void request();

    OrgPlayerResponse *waitResponse();
};


#endif //BEACH_VOLEY_PARTNERREQUESTER_H
