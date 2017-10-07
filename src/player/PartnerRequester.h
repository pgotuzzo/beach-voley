#ifndef BEACH_VOLEY_PARTNERREQUESTER_H
#define BEACH_VOLEY_PARTNERREQUESTER_H
#include <string>
#include "../Definitions.h"

//extern const char *FIFO_FILE_PARTNER_REQUEST;
//extern const char *FIFO_FILE_PARTNER_RESPONSE;

using namespace std;

const int BUFFSIZE = sizeof(int);

class PartnerRequester {

public:
    static ssize_t request(const string &name, pid_t pid);

    static OrgPlayerResponse * waitResponse(string name);
};


#endif //BEACH_VOLEY_PARTNERREQUESTER_H
