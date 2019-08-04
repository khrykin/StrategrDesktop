#ifndef NEWACTIVITYMENU_H
#define NEWACTIVITYMENU_H

#include "colorpicker.h"
#include "models/activity.h"
#include <QColorDialog>
#include <QLineEdit>
#include <QMenu>
#include <QWidgetAction>

class NewActivityMenu : public QMenu {
Q_OBJECT
public:
    explicit NewActivityMenu(QWidget *parent = nullptr);

signals:
    void addNewActivity(const Activity &activity);

public slots:

private:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void createLineEditWidgetAction();
    void createColorWidgetAction();
    void createSaveCancelWidgetAction();

    void colorPickerColorChanged(const std::optional<QColor> &color);

    void saveAndClose();
    void cancelAndClose();
    void reset();

    QWidgetAction *lineEditWidgetAction = nullptr;
    QWidgetAction *colorWidgetAction = nullptr;
    QWidgetAction *saveCancelWidgetAction = nullptr;

    QAction *saveAction = nullptr;
    QAction *customColorAction = nullptr;
    QLineEdit *lineEdit = nullptr;
    QColorDialog *colorDialog = nullptr;
    ColorPicker *colorPicker = nullptr;
    QColor currentColor;
};

#endif // NEWACTIVITYMENU_H
