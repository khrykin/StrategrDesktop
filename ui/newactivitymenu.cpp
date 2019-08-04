#include "newactivitymenu.h"
#include "enterpresseater.h"

#include <QDebug>
#include <QLayout>
#include <QPushButton>

NewActivityMenu::NewActivityMenu(QWidget *parent) : QMenu(parent) {
    createLineEditWidgetAction();
    addSeparator();
    createColorWidgetAction();

    customColorAction = new QAction(tr("Custom Color"));
    customColorAction->setCheckable(true);
    addAction(customColorAction);
    connect(customColorAction, &QAction::triggered, [=]() {
        colorDialog->close();
        colorDialog->show();
        show();
    });

    addAction(customColorAction);
    addSeparator();

    createSaveCancelWidgetAction();

    colorDialog = new QColorDialog(this);
    colorDialog->setOptions(QColorDialog::NoButtons);
    connect(colorDialog, &QColorDialog::currentColorChanged,
            [=](const QColor &color) {
                colorPicker->setColor(color);
                customColorAction->setChecked(true);
                currentColor = color;
            });

    reset();
}

void NewActivityMenu::mouseReleaseEvent(QMouseEvent *event) {
    QAction *action = activeAction();
    if (action && action->isEnabled()) {
        action->trigger();
    }
}

void NewActivityMenu::createLineEditWidgetAction() {
    lineEditWidgetAction = new QWidgetAction(this);
    auto *lineEditWrapper = new QWidget(this);
    auto *lineEditLayout = new QVBoxLayout(lineEditWrapper);
    lineEditLayout->setSpacing(0);
    lineEditWrapper->setLayout(lineEditLayout);

    lineEdit = new QLineEdit(lineEditWrapper);
    lineEdit->setStyleSheet("font-weight: bold;"
                            "font-size: 18pt;"
                            "border: transparent;"
                            "background: transparent;");
    lineEdit->setText("");
    lineEdit->setPlaceholderText("Activity Name");
    lineEdit->setFocus();
    lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);

    auto *enterPressEater = new EnterPressEater(lineEdit);
    lineEdit->installEventFilter(enterPressEater);

    connect(enterPressEater, &EnterPressEater::returnKeyPressed, this,
            &NewActivityMenu::saveAndClose);

    lineEditLayout->addWidget(lineEdit);
    lineEditLayout->setContentsMargins(
            lineEditLayout->contentsMargins().left(), 0,
            lineEditLayout->contentsMargins().right(), 0);

    lineEditWidgetAction->setDefaultWidget(lineEditWrapper);
    addAction(lineEditWidgetAction);
}

void NewActivityMenu::createColorWidgetAction() {
    colorPicker = new ColorPicker(this);
    connect(colorPicker, &ColorPicker::colorChanged, this,
            &NewActivityMenu::colorPickerColorChanged);

    colorWidgetAction = new QWidgetAction(this);
    colorWidgetAction->setDefaultWidget(colorPicker);
    addAction(colorWidgetAction);
}

void NewActivityMenu::createSaveCancelWidgetAction() {
    auto *saveCancelWidget = new QWidget(this);

    saveCancelWidgetAction = new QWidgetAction(this);
    saveCancelWidgetAction->setDefaultWidget(saveCancelWidget);

    auto *layout = new QHBoxLayout(this);
    auto *saveButton = new QPushButton(tr("Save"), this);
    auto *cancelButton = new QPushButton(tr("Cancel"), this);

    saveButton->setDefault(true);

    connect(saveButton, &QPushButton::clicked, this,
            &NewActivityMenu::saveAndClose);
    connect(cancelButton, &QPushButton::clicked, this,
            &NewActivityMenu::cancelAndClose);

    layout->addStretch();
    layout->addWidget(cancelButton);
    layout->addWidget(saveButton);

    saveCancelWidget->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    addAction(saveCancelWidgetAction);
}

void NewActivityMenu::colorPickerColorChanged(
        const std::optional<QColor> &color) {
    customColorAction->setChecked(false);
    if (color) {
        currentColor = *color;
    }
}

void NewActivityMenu::saveAndClose() {
    if (lineEdit->text().isEmpty()) {
        return;
    }

    if (colorDialog->isModal()) {
        return;
    }

    emit addNewActivity(Activity(lineEdit->text().toStdString(),
                                 currentColor.name().toStdString()));
    close();
    reset();
}

void NewActivityMenu::cancelAndClose() {
    close();
    reset();
}

void NewActivityMenu::reset() {
    lineEdit->setText("");
    colorPicker->setRandomColor();
}
