#ifndef SLOT_H
#define SLOT_H

#include "strategy.h"
#include "activitysession.h"
#include "applicationsettings.h"
#include "colorprovider.h"

#include <QMap>
#include <QWidget>

class ActivitySessionWidget : public QWidget, public ColorProvider {
Q_OBJECT

public:
    explicit ActivitySessionWidget(ActivitySession activitySession,
                                   QWidget *parent = nullptr);


    bool isSelected() const;
    void setIsSelected(bool isSelected, bool doUpdate = true);

    void setActivitySession(const ActivitySession &newActivitySession);
    void setSlotHeight(int slotHeight);

    void setSelectBorder(bool isBorderSelected);

    static QColor borderColor();
private:
    int slotHeight = ApplicationSettings::defaultSlotHeight;
    bool _isSelected = false;
    bool isBorderSelected = false;

    ActivitySession activitySession;
    ActivitySession previousActivitySession = ActivitySession();

    Strategy::Duration previousDuration = 0;
    Strategy::Time previousEndTime = 0;

    void drawBorder(QPainter &painter);

    void updateUI();
    int expectedHeight();

    QStringList makeLabelTextComponents() const;

    QColor selectedBackgroundColor() const;
    QColor sessionColor() const;

    void paintEvent(QPaintEvent *event) override;

    void drawSelection(QPainter &painter) const;
    void drawRulers(QPainter &painter) const;
    void drawLabel(QPainter &painter) const;
};


#endif // SLOT_H
