#ifndef ACTIVITYLISTITEMWIDGET_H
#define ACTIVITYLISTITEMWIDGET_H

#include <QColorDialog>
#include <QLineEdit>
#include <QMenu>
#include <QWidget>
#include <QWidgetAction>

#include "activity.h"
#include "activityeditormenu.h"
#include "coloredlabel.h"
#include "colorpicker.h"
#include "colorprovider.h"
#include "dataproviderwidget.h"

class ActivityWidget : public DataProviderWidget, public ColorProvider {
    Q_OBJECT
public:
    explicit ActivityWidget(stg::activity *activity, QWidget *parent);

    stg::activity *activity() const;
    void setActivity(stg::activity *activity);
    bool drawsBorder() const;
    void setDrawsBorder(bool drawsBorder);
    bool isSelected() const;
    void setIsSelected(bool isSelected);

    void choose(QMouseEvent *event = nullptr);
signals:
    void hovered();
    void unhovered();

    void selected();

    void activityEdited(const stg::activity &activity);
    void activityDeleted();

private:
    static constexpr int circleSize = 10;

    stg::activity *_activity;

    ColoredLabel *titleLabel = nullptr;
    ColoredLabel *durationLabel = nullptr;

    ActivityEditorMenu *editorMenu = nullptr;

    bool _isSelected = false;
    bool isClicked = false;
    bool _drawsBorder = true;
    bool activityIsUsed = strategy().time_slots().has_activity(activity());
    int duration = strategy().time_slots().duration_for_activity(activity());

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;
    void paintEvent(QPaintEvent *) override;

    void updateUI();

    void layoutChildWidgets();
    void drawSelection(QPainter &painter) const;
    void drawBorder(QPainter &painter) const;
    void showContextMenu(const QPoint &position);
};

#endif// ACTIVITYLISTITEMWIDGET_H
