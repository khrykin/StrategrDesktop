//
// Created by Dmitry Khrykin on 2019-08-05.
//

#ifndef STRATEGR_COCOALINEEDIT_H
#define STRATEGR_COCOALINEEDIT_H

#include "QtCore"

#ifdef Q_OS_MAC

#include <QMacCocoaViewContainer>

class CocoaLineEdit : public QMacCocoaViewContainer {
public:
    explicit CocoaLineEdit(QWidget *parent = nullptr);

    void setText(const QString &text);
    QString text();
};

#endif

#endif //STRATEGR_COCOALINEEDIT_H
