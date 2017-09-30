#ifndef BEACH_VOLEY_INITEXCEPTION_H
#define BEACH_VOLEY_INITEXCEPTION_H

#include <exception>
#include <string>

using namespace std;

class InitException : public exception {
public:
    InitException(const string &description);

    const char *what() const throw() override;

private:
    const char *description;
};


#endif //BEACH_VOLEY_INITEXCEPTION_H
