#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QTime>

QString timeStringForMins(int mins);
int minutesFromQTime(const QTime &time);
QTime qTimeFromMinutes(int minutes);
#endif // UTILS_H
