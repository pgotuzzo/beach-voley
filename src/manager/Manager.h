//
// Created by sebastian on 07/10/17.
//

#ifndef BEACH_VOLEY_MANAGER_H
#define BEACH_VOLEY_MANAGER_H


class Manager {

public:
    Manager();
    void receiveTask();

private:
    void findPartner(int pid);
    int fd;
};


#endif //BEACH_VOLEY_MANAGER_H
