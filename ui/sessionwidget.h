#ifndef SESSION_WIDGET_H
#define SESSION_WIDGET_H

#include "Strategy.h"
#include "Session.h"
#include "applicationsettings.h"
#include "colorprovider.h"

#include <QMap>
#include <QWidget>

class SessionWidget : public QWidget, public ColorProvider {
Q_OBJECT

public:
    explicit SessionWidget(Session activitySession,
                           QWidget *parent = nullptr);


    bool isSelected() const;
    void setIsSelected(bool isSelected, bool doUpdate = true);

    void setActivitySession(const Session &newActivitySession);
    void setSlotHeight(int slotHeight);

    void setSelectBorder(bool isBorderSelected);

    static QColor borderColor();
private:
    int slotHeight = ApplicationSettings::defaultSlotHeight;
    bool _isSelected = false;
    bool isBorderSelected = false;

    Session activitySession;
    Session previousActivitySession = Session();

    Strategy::Duration previousDuration = 0;
    Strategy::Time previousEndTime = 0;

    void drawBorder(QPainter &painter);

    void updateUI();
    int expectedHeight();

    QColor selectedBackgroundColor() const;
    QColor sessionColor() const;

    void paintEvent(QPaintEvent *event) override;

    void drawSelection(QPainter &painter) const;
    void drawRulers(QPainter &painter) const;
    void drawLabel(QPainter &painter) const;
};


#endif // SESSION_WIDGET_H
