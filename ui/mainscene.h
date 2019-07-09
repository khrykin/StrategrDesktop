//
// Created by Dmitry Khrykin on 2019-07-09.
//

#ifndef STRATEGR_MAINSCENE_H
#define STRATEGR_MAINSCENE_H

#include <QWidget>
#include "strategy.h"

class MainScene : public QWidget {
Q_OBJECT
public:
    explicit MainScene(Strategy &strategy,
                       QWidget *parent = nullptr);

    void showActivitiesList();
    void focusOnCurrentTime();
    void openStrategySettings();

    void setStrategy(const Strategy &newStrategy);

    void updateUI();
private:
    Strategy &strategy;
};

#endif //STRATEGR_MAINSCENE_H
