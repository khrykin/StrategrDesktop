#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <string>

struct Activity {
  Activity(std::string name, std::string color = "#000000");
  std::string name;
  std::string color;

  friend bool operator==(const Activity &a, const Activity &b) {
    return a.name == b.name && a.color == b.color;
  }

  friend bool operator!=(const Activity &a, const Activity &b) {
    return !(a == b);
  }
};

#endif // ACTIVITY_H
