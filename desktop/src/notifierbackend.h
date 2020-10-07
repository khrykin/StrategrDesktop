#ifndef NOTIFIERBACKEND_H
#define NOTIFIERBACKEND_H

#include <QString>

class NotifierBackend {
public:
    NotifierBackend();
    void sendMessage(const QString &title, const QString &message) const;
};


#endif // NOTIFIERBACKEND_H
