#ifndef ACTIVITYLISTITEMWIDGET_H
#define ACTIVITYLISTITEMWIDGET_H

#include <QColorDialog>
#include <QLineEdit>
#include <QMenu>
#include <QWidget>
#include <QWidgetAction>

#include "colorpicker.h"
#include "activity.h"
#include "activityeditormenu.h"
#include "colorprovider.h"
#include "coloredlabel.h"

class ActivityListItemWidget : public QWidget, public ColorProvider {
Q_OBJECT
public:
    explicit ActivityListItemWidget(Activity *activity,
                                    QWidget *parent = nullptr);

    Activity *activity() const;
    void setActivity(Activity *activity);
    bool drawsBorder() const;
    void setDrawsBorder(bool drawsBorder);
signals:
    void hovered();
    void unhovered();

    void selected();

    void activityEdited(const Activity &activity);
    void activityDeleted();
private:
    Activity *_activity;

    ColoredLabel *label = nullptr;
    ActivityEditorMenu *editorMenu = nullptr;

    bool isClicked = false;
    bool _drawsBorder = true;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;
    void paintEvent(QPaintEvent *) override;

    void updateUI();

    void layoutChildWidgets();
    void setupActions();
    void drawSelection(QPainter &painter) const;
    void drawBorder(QPainter &painter) const;
};

#endif // ACTIVITYLISTITEMWIDGET_H
