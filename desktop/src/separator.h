//
// Created by Dmitry Khrykin on 2019-08-05.
//

#ifndef STRATEGR_SEPARATOR_H
#define STRATEGR_SEPARATOR_H

#include <QWidget>

class Separator : public QWidget {
public:
    explicit Separator(QWidget *parent = nullptr);

private:
    void paintEvent(QPaintEvent *) override;
};


#endif//STRATEGR_SEPARATOR_H
