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

class ActivityWidget : public QWidget, public ColorProvider {
Q_OBJECT
public:
    explicit ActivityWidget(stg::activity *activity,
                            QWidget *parent = nullptr);

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
    stg::activity *_activity;

    ColoredLabel *label = nullptr;
    ActivityEditorMenu *editorMenu = nullptr;

    bool _isSelected = false;
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
    void drawSelection(QPainter &painter) const;
    void drawBorder(QPainter &painter) const;
    void showContextMenu(const QPoint &position);
};

#endif // ACTIVITYLISTITEMWIDGET_H
