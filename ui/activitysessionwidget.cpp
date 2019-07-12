#include "activitysessionwidget.h"
#include "third-party/stacklayout.h"
#include "utils.h"
#include <QDebug>
#include <QLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <models/strategy.h>

ActivitySessionWidget::ActivitySessionWidget(const ActivitySession *activitySession,
                                             QWidget *parent)
        : activitySession(activitySession),
          QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);

    layoutChildWidgets();

    updateUI();
    updateStyleSheet();
}

void ActivitySessionWidget::layoutChildWidgets() {
    auto *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    titleLabel = new QLabel();
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 15pt;"
                              "font-weight: bold;");

    layout()->addWidget(titleLabel);
}

void ActivitySessionWidget::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}


void ActivitySessionWidget::updateStyleSheet() {
    auto leftBorderStyle = activitySession->activity
                           ? "border-left: 3px solid "
                             + QString::fromStdString(activitySession->activity->color())
                             + ";"
                           : "";

    if (isSelected()) {
        setStyleSheet("ActivitySessionWidget { "
                      "background-color: #efefef;" +
                      leftBorderStyle +
                      "border-bottom: 1px solid #ececec;"
                      "}");
    } else {
        if (activitySession->activity) {
            setStyleSheet("ActivitySessionWidget { "
                          "background-color: white;" +
                          leftBorderStyle +
                          "border-bottom: 1px solid #ececec;"
                          "}");
        } else {
            setStyleSheet("QLabel { "
                          "background-color: #f4f4f4;"
                          "border-bottom: 1px solid #ececec;"
                          "}");
        }
    }
}

bool ActivitySessionWidget::isSelected() const {
    return _isSelected;
}

void ActivitySessionWidget::setIsSelected(bool isSelected, bool doUpdate) {
    _isSelected = isSelected;

    if (doUpdate) {
        updateStyleSheet();
    }
}

void ActivitySessionWidget::updateUI() {
    auto durationChanged = height() != expectedHeight();
    auto activityChanged = previousActivity != activitySession->activity;

    if (!activityChanged && !durationChanged) {
        return;
    }

    if (durationChanged) {
        setFixedHeight(expectedHeight());
    }

    if (activityChanged) {
        updateStyleSheet();
    }

    updateLabel();

    previousActivity = activitySession->activity;
}

void ActivitySessionWidget::updateLabel() const {
    if (activitySession->activity) {
        titleLabel->setText(labelText());
    } else {
        titleLabel->setText("");
    }
}

QString ActivitySessionWidget::labelText() const {
    auto activity = activitySession->activity;

    auto name = QString::fromStdString(activity->name());
    auto color = QString::fromStdString(activity->color());
    auto time = humanTimeForMinutes(activitySession->duration());

    return QString("<font color=\"#888\"><b>%1</b></font>"
                   " "
                   "<font color=\"%2\"><b>%3</b></font>")
            .arg(time).arg(color).arg(name);
}

void ActivitySessionWidget::setActivitySession(const ActivitySession *newActivitySession) {
    activitySession = newActivitySession;
    updateUI();
}

void ActivitySessionWidget::setSlotHeight(int newSlotHeight) {
    slotHeight = newSlotHeight;
}

int ActivitySessionWidget::expectedHeight() {
    return activitySession->length() * slotHeight;
}