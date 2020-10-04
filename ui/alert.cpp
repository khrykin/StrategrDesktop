//
// Created by Dmitry Khrykin on 2019-07-13.
//

#include "alert.h"
#include <QLayout>
#include <QMessageBox>

#ifndef Q_OS_MAC

int Alert::showAskToSave(const QString &title, const QString &message) {
    QMessageBox messageBox;
    messageBox.setText(title);

    messageBox.setInformativeText(message);

    messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    messageBox.setDefaultButton(QMessageBox::Save);

    return messageBox.exec();
}

int Alert::showWarning(QWidget *parent,
                       const QString &title,
                       const QString &message) {
    QMessageBox::warning(parent, title, message);
    return 0;
}

#endif
