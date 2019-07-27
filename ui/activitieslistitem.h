#ifndef ACTIVITIESLISTITEM_H
#define ACTIVITIESLISTITEM_H

#include <QColorDialog>
#include <QLineEdit>
#include <QMenu>
#include <QWidget>
#include <QWidgetAction>

#include "colorpicker.h"
#include "activity.h"
#include "newactivitymenu.h"
#include "colorprovider.h"
#include "coloredlabel.h"

class ActivitiesListItemWidget : public QWidget, public ColorProvider {
Q_OBJECT
public:
    explicit ActivitiesListItemWidget(Activity *activity,
                                      QWidget *parent = nullptr);

    Activity *activity() const;
    void setActivity(Activity *activity);

signals:
    void selected();
    void wantToDelete();
    void wantToEdit();
    void activityEdited(const Activity &activity);

private:
    Activity *_activity;

    QAction *editActivityAction = nullptr;
    QAction *deleteActivityAction = nullptr;
    QAction *customColorAction = nullptr;

    QWidgetAction *colorWidgetAction = nullptr;
    QWidgetAction *lineEditWidgetAction = nullptr;

    QMenu *contextMenu = nullptr;
    QColorDialog *colorDialog = nullptr;
    QLineEdit *lineEdit = nullptr;
    ColorPicker *colorPicker = nullptr;
    ColoredLabel *label = nullptr;

    QColor previousColor;
    bool isClicked = false;
    bool isHovered = false;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;
    void paintEvent(QPaintEvent *) override;

    void wantDeleteActivity();
    void wantEditActivity();
    void wantToSetCustomColor();

    void createLineEditWidgetAction();
    void createColorWidgetAction();

    void editActivityColor(const QColor &color);
    void editActivityNameFromLineEdit();

    void colorPickerColorChanged(const std::optional<QColor> &color);
    void colorDialogRejected();
    void updateUI();

    void layoutChildWidgets();
    void setupActions();
    QMenu *makeContextMenu();
    void updateStyleSheet();
};

#endif // ACTIVITIESLISTITEM_H
