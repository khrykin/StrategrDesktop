#include "utils.h"

QString timeStringForMins(int mins) {
  if (mins < 1) {
    return "Less than 1 min";
  }

  int hours = mins / 60;
  int minsRemainder = mins - hours * 60;

  QString result;

  if (hours > 0) {
    result += QString::number(hours);
  }

  if (hours > 0 && minsRemainder == 30) {
    result += ".5 h";
  } else if (hours > 0) {
    result += " h";
  }

  if (minsRemainder != 0) {
    if (hours > 0) {
      if (minsRemainder == 30) {
        return result;
      }

      result += " ";
    }

    result += QString::number(minsRemainder) + " min";
  }

  return result;
}
