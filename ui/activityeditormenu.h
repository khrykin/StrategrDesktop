#ifndef ACTIVITYEDITORMENU_H
#define ACTIVITYEDITORMENU_H

#include <QColorDialog>
#include <QLineEdit>
#include <QMenu>
#include <QWidgetAction>

#include "activity.h"
#include "coloredlabel.h"
#include "colorpicker.h"

class ActivityEditorMenu : public QMenu {
    Q_OBJECT
public:
    explicit ActivityEditorMenu(std::optional<stg::activity> activity = std::nullopt,
                                QWidget *parent = nullptr);

    void focus();

signals:
    void submitActivity(const stg::activity &activity);
    void deleteActivity();

private:
    std::optional<stg::activity> activity;

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

    int platformPadding =
#ifdef Q_OS_WIN32
        20
#else
        0
#endif
        ;

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

#endif// ACTIVITYEDITORMENU_H
