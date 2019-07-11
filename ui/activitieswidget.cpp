//
// Created by Dmitry Khrykin on 2019-07-10.
//

#include "activitieswidget.h"

ActivitiesWidget::ActivitiesWidget(QWidget *parent) : QWidget(parent) {
    setupActions();
    setLayout(new QVBoxLayout());
    layout()->setSpacing(0);

    auto label = new QLabel("ActivitiesWidget");
    label->setAlignment(Qt::AlignCenter);

    layout()->addWidget(label);
}

void ActivitiesWidget::getBack() {
    emit wantToGetBack();
}

void ActivitiesWidget::setupActions() {
    auto *getBackAction = new QAction("Back", this);
    getBackAction->setShortcut(QKeySequence(Qt::Key_Escape));

    connect(getBackAction,
            &QAction::triggered,
            this,
            &ActivitiesWidget::getBack);

    addAction(getBackAction);
}
