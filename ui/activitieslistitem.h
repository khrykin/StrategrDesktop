#ifndef ACTIVITIESLISTITEM_H
#define ACTIVITIESLISTITEM_H

#include "colorpicker.h"
#include "models/activity.h"
#include "newactivitymenu.h"
#include <QColorDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QWidget>
#include <QWidgetAction>

class ActivitiesListItem : public QWidget {
Q_OBJECT
public:
    explicit ActivitiesListItem(Activity *activity,
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
    QLabel *label = nullptr;

    QColor previousColor;
    bool _isClicked = false;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
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
};

#endif // ACTIVITIESLISTITEM_H
