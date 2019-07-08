//
// Created by Dmitry Khrykin on 2019-07-08.
//

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>

class MainWindow : public QWidget {
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setLayout(new QHBoxLayout());

        _button = new QPushButton("click me fuckka");
        layout()->addWidget(_button);

        connect(_button, &QPushButton::click, [=]() {
            _button->setText("bitch you did it");
        });
    }

    QPushButton *button() const {
        return _button;
    }

private:
    QPushButton *_button;
};


#endif //UI_MAINWINDOW_H
