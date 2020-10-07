//
// Created by Dmitry Khrykin on 2019-07-13.
//

#ifndef STRATEGR_ALERT_H
#define STRATEGR_ALERT_H

#include <QString>
#include <QWidget>

class Alert {
public:
    static int showAskToSave(const QString &title, const QString &message);
    static int showWarning(QWidget *parent,
                           const QString &title,
                           const QString &message = "");
};


#endif//STRATEGR_ALERT_H
