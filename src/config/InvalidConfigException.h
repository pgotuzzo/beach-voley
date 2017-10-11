//
// Created by sebastian on 11/10/17.
//

#ifndef BEACH_VOLEY_INVALIDCONFIGEXCEPTION_H
#define BEACH_VOLEY_INVALIDCONFIGEXCEPTION_H

#include <exception>
#include <cstring>
#include <string>

using namespace std;

class InvalidConfigException : public exception{

public:
    explicit InvalidConfigException(const string &description);

    const char *what() const throw() override;

private:
    const char *description;

};


#endif //BEACH_VOLEY_INVALIDCONFIGEXCEPTION_H
