#ifndef ACTIVITYEDITORMENU_H
#define ACTIVITYEDITORMENU_H

#include <QColorDialog>
#include <QLineEdit>
#include <QMenu>
#include <QWidgetAction>

#include "colorpicker.h"
#include "activity.h"
#include "coloredlabel.h"

class ActivityEditorMenu : public QMenu {
Q_OBJECT
public:
    explicit ActivityEditorMenu(std::optional<Activity> activity = std::nullopt,
                                QWidget *parent = nullptr);

signals:
    void submitActivity(const Activity &activity);
    void deleteActivity();

private:
    std::optional<Activity> activity;

    QWidgetAction *lineEditWidgetAction = nullptr;
    QWidgetAction *colorWidgetAction = nullptr;
    QWidgetAction *saveCancelWidgetAction = nullptr;
    QWidgetAction *errorWidgetAction = nullptr;

    QAction *customColorAction = nullptr;

    QLineEdit *lineEdit = nullptr;
    ColoredLabel *errorLabel = nullptr;
    QColorDialog *colorDialog = nullptr;
    ColorPicker *colorPicker = nullptr;

    QColor currentColor;

    void addLineEditWidgetAction();
    void addColorWidgetAction();
    void addSaveCancelWidgetAction();
    void addErrorWidgetAction();
    void addCustomColorAction();

    void setupColorDialog();

    void colorPickerColorChanged(const std::optional<QColor> &color);
    void colorPickerClicked();

    void saveAndClose();
    void cancelAndClose();
    void deleteAndClose();
    void reset();

    QString defaultLineEditText() const;

    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // ACTIVITYEDITORMENU_H