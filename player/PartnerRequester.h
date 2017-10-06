//
// Created by sebastian on 03/10/17.
//

#ifndef BEACH_VOLEY_PARTNERREQUESTER_H
#define BEACH_VOLEY_PARTNERREQUESTER_H
#include <string>

using namespace std;

class PartnerRequester {

public:
    static ssize_t request(const string &name, pid_t pid);

};


#endif //BEACH_VOLEY_PARTNERREQUESTER_H
