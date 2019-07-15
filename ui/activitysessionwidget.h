#ifndef SLOT_H
#define SLOT_H

#include <QLabel>
#include <QMap>
#include <QWidget>
#include <models/strategy.h>

#include "activitysession.h"
#include "applicationsettings.h"

class ActivitySessionWidget : public QWidget {
Q_OBJECT

public:
    explicit ActivitySessionWidget(const ActivitySession &activitySession,
                                   QWidget *parent = nullptr);


    bool isSelected() const;
    void setIsSelected(bool isSelected, bool doUpdate = true);

    void setActivitySession(const ActivitySession &newActivitySession);
    void setSlotHeight(int slotHeight);

private:
    int slotHeight = ApplicationSettings::defaultSlotHeight;
    bool _isSelected = false;

    ActivitySession activitySession;
    ActivitySession previousActivitySession = ActivitySession();
    int previousDuration = 0;

    QLabel *label = nullptr;
    QLabel *titleLabel = nullptr;

    void paintEvent(QPaintEvent *event) override;

    void updateStyleSheet();
    void layoutChildWidgets();

    void updateUI();
    int expectedHeight();
    void updateLabel() const;
    QString labelText() const;
};

#endif // SLOT_H
