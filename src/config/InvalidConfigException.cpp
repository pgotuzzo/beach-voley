//
// Created by sebastian on 11/10/17.
//
#include <cstring>
#include "InvalidConfigException.h"


using namespace std;

InvalidConfigException::InvalidConfigException(const string &description) {
    char *temp = new char[description.length() + 1];
    std::strcpy(temp, description.c_str());
    this->description = temp;
}

const char *InvalidConfigException::what() const throw() {
    return description;
}