#include "ParseException.h"

using namespace std;

ParseException::ParseException(const string &description) {
    char *temp = new char[description.length() + 1];
    std::strcpy(temp, description.c_str());
    this->description = temp;
}

const char *ParseException::what() const throw() {
    return description;
}