//
// Created by Dmitry Khrykin on 2019-07-10.
//

#ifndef STRATEGR_ACTIVITIESWIDGET_H
#define STRATEGR_ACTIVITIESWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QAction>

class ActivitiesWidget : public QWidget {
Q_OBJECT
public:
    explicit ActivitiesWidget(QWidget *parent = nullptr);

signals:
    void wantToGetBack();

private:
    void getBack();
    void setupActions();
};


#endif //STRATEGR_ACTIVITIESWIDGET_H
