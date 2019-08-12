//
// Created by Dmitry Khrykin on 2019-08-05.
//

#ifndef STRATEGR_VISUALEFFECTWIDGET_H
#define STRATEGR_VISUALEFFECTWIDGET_H

#include "QtCore"

#ifdef Q_OS_MAC

#include <QMacCocoaViewContainer>

class VisualEffectWidget : public QMacCocoaViewContainer {
public:
    explicit VisualEffectWidget(QWidget *parent = nullptr);

    void setChildWidget(QWidget *widget);
};

#else
class VisualEffectWidget : public QWidget {
};
#endif

#endif //STRATEGR_VISUALEFFECTWIDGET_H
