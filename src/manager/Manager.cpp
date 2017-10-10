#include <cstring>
#include "Manager.h"
#include "../config/Constants.h"
#include "../../util/ResourceHandler.h"

const char *TAG = "Manager: ";

Manager::Manager() {
    fifoRead = ResourceHandler::getInstance()->createFifoRead(FIFO_FILE_MANAGER_RECEIVE_TASK);
}

/**
 * Creates a separated process and starts to receive task.
 */
void Manager::initManager(){
    int pid = fork();
    if (pid == 0) {
        this->receiveTask();
        exit(0);
    }
}

void Manager::receiveTask() {
    count++;
    cout << TAG << "Trying to open a fifo to read a task N°: " << count << endl;
    int fd = fifoRead->openFifo();
    if (fd < 0) {
        stringstream message;
        message << TAG << "Trying to open a fifo to read a task. Fifo couldn't be opened. Error Number: "
                << strerror(errno) << " tamaño: " << count << endl;
        throw runtime_error(message.str());
    }

    TaskRequest task = {};
    cout << TAG << "Trying to read a task" << endl;
    ssize_t out = fifoRead->readFifo((&task), sizeof(TaskRequest));
    cout << TAG << "Read something... :thinking: " << task.show() << " out:" << out << endl;
    fifoRead->closeFifo();

    if (out > 0) {
        cout << TAG << "Read a task successfully!" << endl;
        switch (task.task) {
            case (FIND_PARTNER):
                findPartner(task.pid);
                break;
            case (MATCH_CANCELLED):break;
            case (MATCH_RESULT):break;
            default:
                throw runtime_error("Task handler not defined.");
        }
        cout << TAG << "Task completed! Going for a new one" << endl;
    }

    receiveTask();
}

void Manager::findPartner(int pid) {
    //TODO: find partner + find stadium
    if (mFifoWrite.find(pid) == mFifoWrite.end()) {
        FifoWrite *f = ResourceHandler::getInstance()->createFifoWrite(FIFO_FILE_PARTNER_RESPONSE + to_string(pid));
        int fd = f->openFifo();
        if (fd < 0) {
            stringstream message;
            message << TAG << "Trying to open a fifo to write a response. Fifo couldn't be opened. Error Number: " << strerror(errno) << " " <<errno << " Tamaño:" << mFifoWrite.size();
            throw runtime_error(message.str());
        }
        mFifoWrite.emplace(pid, *f);
    }
    FifoWrite fifoWrite = mFifoWrite[pid];

    cout << TAG << "A partner for process: " << pid << "was found!!" << endl;
    OrgPlayerResponse response = {0, 0, ENUM_PLAY};
    cout << TAG << "Trying to write a response" << endl;
    ssize_t out = fifoWrite.writeFifo(static_cast<const void *> (&response), sizeof(OrgPlayerResponse));
    if (out < 0) {
        throw runtime_error("Partner response fifo can't be write!");
    }
    cout << TAG << "Response written successfully" << endl;
}


