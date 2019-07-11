//
// Created by Dmitry Khrykin on 2019-07-10.
//

#ifndef STRATEGR_CURRENTSESSIONWIDGET_H
#define STRATEGR_CURRENTSESSIONWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QLabel>

class CurrentSessionWidget : public QWidget {
public:
    explicit CurrentSessionWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setLayout(new QHBoxLayout());

        auto label = new QLabel("CurrentSessionWidget");
        label->setAlignment(Qt::AlignCenter);

        layout()->addWidget(label);
    }
};

#endif //STRATEGR_CURRENTSESSIONWIDGET_H
