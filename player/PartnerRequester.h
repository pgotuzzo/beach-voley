//
// Created by sebastian on 03/10/17.
//

#ifndef BEACH_VOLEY_PARTNERREQUESTER_H
#define BEACH_VOLEY_PARTNERREQUESTER_H
#include <string>

using namespace std;

const int BUFFSIZE = sizeof(pid_t);

class PartnerRequester {

public:
    static ssize_t request(const string &name, pid_t pid);

    static void waitResponse(string name);
};


#endif //BEACH_VOLEY_PARTNERREQUESTER_H
