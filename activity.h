#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <string>

struct Activity
{
    Activity(std::string name);
    std::string name;

//    bool operator==(const Activity& other) {
//        return name == other.name;
//    }

//    bool operator!=(const Activity& other);

    friend bool operator==(const Activity& a, const Activity& b) {
        return a.name == b.name;
    }

    friend bool operator!=(const Activity& a, const Activity& b) {
        return !(a == b);
    }
};

#endif // ACTIVITY_H
