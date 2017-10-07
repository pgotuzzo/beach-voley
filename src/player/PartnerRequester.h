//
// Created by sebastian on 03/10/17.
//

#ifndef BEACH_VOLEY_PARTNERREQUESTER_H
#define BEACH_VOLEY_PARTNERREQUESTER_H
#include <string>
#include "../Definitions.h"

using namespace std;

const int BUFFSIZE = sizeof(int);

class PartnerRequester {

public:
    static ssize_t request(const string &name, pid_t pid);

    static OrgPlayerResponse * waitResponse(string name);
};


#endif //BEACH_VOLEY_PARTNERREQUESTER_H
