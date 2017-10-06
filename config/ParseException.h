#ifndef BEACH_VOLEY_PARSEEXCEPTION_H
#define BEACH_VOLEY_PARSEEXCEPTION_H

#include <exception>
#include <cstring>
#include <string>

using namespace std;

class ParseException : public exception {
public:
    explicit ParseException(const string &description);

    const char* what() const throw() override;

private:
    const char *description;
};


#endif //BEACH_VOLEY_PARSEEXCEPTION_H
