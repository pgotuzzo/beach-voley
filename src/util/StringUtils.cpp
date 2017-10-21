#include "StringUtils.h"

using namespace std;

string toString(const char *aCharPtr[], int count) {
    string s;
    for (int i = 0; i < count; i++) {
        s += aCharPtr[i];
        if (i != count - 1) {
            s += " ";
        }
    }
    return s;
}

string toString(const vector<string> v) {
    string s;
    for (auto &item : v) {
        s += item + " ";
    }
    return s.substr(0, s.size() - 1);
}