#include <cstring>
#include "InitException.h"

using namespace std;

const char *InitException::what() const throw() {
    return description;
}

InitException::InitException(const string &description) {
    char *temp = new char[description.length() + 1];
    std::strcpy(temp, description.c_str());
    this->description = temp;
}
