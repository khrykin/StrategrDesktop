#ifndef SESSION_WIDGET_H
#define SESSION_WIDGET_H

#include "applicationsettings.h"
#include "colorprovider.h"
#include "dataproviderwidget.h"
#include "session.h"
#include "strategy.h"

#include <QMap>
#include <QWidget>

class SessionWidget : public DataProviderWidget, public ColorProvider {
public:
    static QColor borderColor();
    explicit SessionWidget(const stg::session &session, QWidget *parent);

    void setIsSelected(bool isSelected);
    void setIsBorderSelected(bool isBorderSelected);
    void setSession(const stg::session &newSession);
    void setDrawsBorders(bool drawsBorders);

private:
    bool _isSelected = false;
    bool _isBorderSelected = false;
    bool _drawsBorders = true;

    stg::session session;
    stg::session previousSession = stg::session();

    stg::strategy::duration_t previousDuration = 0;
    stg::strategy::time_t previousEndTime = 0;

    void reloadSession();
    int expectedHeight();
    int topMargin();

    QColor selectedBackgroundColor() const;
    QColor sessionColor() const;

    void paintEvent(QPaintEvent *event) override;

    void drawBorder(QPainter &painter);
    void drawBackground(QPainter &painter);
    void drawRulers(QPainter &painter);
    void drawLabel(QPainter &painter);
};


#endif// SESSION_WIDGET_H
