#ifndef BEACH_VOLEY_PLAYER_H
#define BEACH_VOLEY_PLAYER_H

#include <string>

using namespace std;
extern const char* FIFO_FILE_TOURNAMENT;
extern const char* FIFO_FILE_PARTNER_REQUEST;
extern const char* FIFO_FILE_PARTNER_RESPONSE;


class Player {

private:
    string name;
public:
    Player(const string &name);

    void subscribe();
    void partnerRequest();
    void organizatorResponse();

    int createTempFile();

    int createTempFile(string fileName);

    void removeTmpFile(string basic_string);
};


#endif //BEACH_VOLEY_PLAYER_H
