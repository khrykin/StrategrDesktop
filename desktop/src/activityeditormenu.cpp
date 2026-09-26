#include <QLayout>
#include <QPushButton>

#include "activityeditormenu.h"
#include "alert.h"
#include "applicationsettings.h"
#include "colorprovider.h"
#include "colorutils.h"
#include "enterpresseater.h"
#include "utils.h"

ActivityEditorMenu::ActivityEditorMenu(std::optional<stg::activity> activity,
                                       QWidget *parent)
    : QMenu(parent), activity(std::move(activity)) {
    addLineEditWidgetAction();

    addSeparator();
    addErrorWidgetAction();

    addColorWidgetAction();
    addCustomColorAction();

    addSeparator();

    if (this->activity) {
        addAction(tr("Delete"), this, &ActivityEditorMenu::deleteAndClose);
    } else {
        addSaveCancelWidgetAction();
    }

    setupColorDialog();
}

void ActivityEditorMenu::setupColorDialog() {
    using namespace ColorUtils;

    colorDialog = new QColorDialog(this);
    colorDialog->setOptions(QColorDialog::NoButtons);

    connect(colorDialog, &QColorDialog::currentColorChanged,
            [=](const QColor &color) {
                colorPicker->setColor(color);
                customColorAction->setChecked(!colorPicker->color());
                currentColor = color;

                if (activity && QColorFromStdString(activity->color()) != currentColor) {
                    colorPickerClicked();
                }
            });
}

void ActivityEditorMenu::addCustomColorAction() {
    customColorAction = new QAction(tr("Custom Color"));
    customColorAction->setCheckable(true);

    customColorAction->setChecked(!colorPicker->color());

    connect(customColorAction, &QAction::triggered, [=]() {
        if (activity) {
            colorDialog->setCurrentColor(activity->color());
        } else if (colorPicker->color()) {
            colorDialog->setCurrentColor(*colorPicker->color());
        }

        colorDialog->close();
        colorDialog->show();
        show();

        customColorAction->setChecked(!colorPicker->color());
    });

    addAction(customColorAction);
}

void ActivityEditorMenu::addErrorWidgetAction() {
    errorWidgetAction = new QWidgetAction(this);

    errorLabel = new ColoredLabel("");
    errorLabel->setFontHeight(12);
    errorLabel->setDynamicColor(&ColorProvider::secondaryTextColor);

    auto wrapper = new QWidget();
    wrapper->setLayout(new QHBoxLayout());
    wrapper->layout()->setSpacing(0);
    wrapper->layout()->setContentsMargins(ApplicationSettings::defaultPadding + 5,
                                          0,
                                          ApplicationSettings::defaultPadding + 5,
                                          2);
    wrapper->layout()->addWidget(errorLabel);

    errorWidgetAction->setDefaultWidget(wrapper);
    errorWidgetAction->setVisible(false);

    addAction(errorWidgetAction);
}

void ActivityEditorMenu::mouseReleaseEvent(QMouseEvent *event) {
    QAction *action = activeAction();
    if (action && action->isEnabled() && !qobject_cast<QWidgetAction *>(action)) {
        action->trigger();
    } else {
        QMenu::mouseReleaseEvent(event);
    }
}

void ActivityEditorMenu::addLineEditWidgetAction() {
    lineEditWidgetAction = new QWidgetAction(this);

    // No wrapper/layout here on purpose: a QVBoxLayout was previously used
    // just to add a left margin, but under Qt6 its cached geometry can go
    // stale relative to the line edit's forced minimum height, leaving the
    // line edit positioned outside its own parent's bounds and clipped to
    // invisibility. QLineEdit's own text margins give the same padding
    // without an intermediate layout to desync.
    lineEdit = new QLineEdit(this);
    lineEdit->setStyleSheet(QString("font-weight: bold;"
                                    "font-size: 18px;"
                                    "border: transparent;"
                                    "background: transparent;"
                                    "color: %1;")
                                .arg(ColorUtils::qColorToCSS(ColorProvider::textColor())));
    lineEdit->setText(defaultLineEditText());
    lineEdit->setPlaceholderText("Activity Name");
    lineEdit->setFocus();
    lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
    lineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    lineEdit->setTextMargins(12 + platformPadding, 0, 12, 0);

    // The stylesheet's "border: transparent" leaves Qt6's QLineEdit size
    // hint with no usable frame metrics, collapsing its computed height to
    // 0. Force a sane fixed height explicitly instead of relying on it.
    QFont lineEditFont;
    lineEditFont.setPixelSize(18);
    lineEditFont.setBold(true);
    lineEdit->setFixedHeight(QFontMetrics(lineEditFont).height() + 12);

    auto *enterPressEater = new EnterPressEater(lineEdit);
    lineEdit->installEventFilter(enterPressEater);

    connect(enterPressEater,
            &EnterPressEater::returnKeyPressed,
            this,
            &ActivityEditorMenu::saveAndClose);

    lineEditWidgetAction->setDefaultWidget(lineEdit);
    addAction(lineEditWidgetAction);
}

QString ActivityEditorMenu::defaultLineEditText() const {
    return activity
               ? QString::fromStdString(activity->name())
               : "";
}

void ActivityEditorMenu::addColorWidgetAction() {
    using namespace ApplicationSettings;
    using namespace ColorUtils;

    colorPicker = new ColorPicker(this);
    colorPicker->setContentsMargins(defaultPadding + 2 + platformPadding,
                                    0,
                                    defaultPadding,
                                    defaultPadding / 2);

    if (activity) {
        auto activityColor = QColorFromStdString(activity->color());
        colorPicker->setColor(activityColor);
        currentColor = activityColor;
    } else {
        colorPicker->setRandomColor();
        currentColor = *colorPicker->color();
    }


    connect(colorPicker,
            &ColorPicker::colorChanged,
            this,
            &ActivityEditorMenu::colorPickerColorChanged);

    connect(colorPicker,
            &ColorPicker::colorChangedByClick,
            this,
            &ActivityEditorMenu::colorPickerClicked);

    colorWidgetAction = new QWidgetAction(this);
    colorWidgetAction->setDefaultWidget(colorPicker);

    addAction(colorWidgetAction);
}

void ActivityEditorMenu::addSaveCancelWidgetAction() {
    auto *saveCancelWidget = new QWidget(this);

    saveCancelWidgetAction = new QWidgetAction(this);
    saveCancelWidgetAction->setDefaultWidget(saveCancelWidget);

    auto *layout = new QHBoxLayout(this);
    auto *saveButton = new QPushButton(tr("Save"), this);
    saveButton->setDefault(true);

    auto *cancelButton = new QPushButton(tr("Cancel"), this);

    connect(saveButton,
            &QPushButton::clicked,
            this,
            &ActivityEditorMenu::saveAndClose);
    connect(cancelButton,
            &QPushButton::clicked,
            this,
            &ActivityEditorMenu::cancelAndClose);

    layout->addStretch();
    layout->addWidget(cancelButton);
    layout->addWidget(saveButton);

    saveCancelWidget->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    addAction(saveCancelWidgetAction);
}

void ActivityEditorMenu::colorPickerColorChanged(
    const std::optional<QColor> &color) {
    customColorAction->setChecked(false);

    if (color) {
        currentColor = *color;
    }
}

void ActivityEditorMenu::saveAndClose() {
    if (lineEdit->text().isEmpty()) {
        return;
    }

    if (colorDialog->isModal()) {
        return;
    }

    try {
        emit submitActivity(stg::activity(lineEdit->text().toStdString(), currentColor));
        close();
        reset();
    } catch (const std::exception &e) {
        errorLabel->setText(StringUtils::toSentenceCase(e.what()));
        errorWidgetAction->setVisible(true);
    }
}

void ActivityEditorMenu::cancelAndClose() {
    close();
    reset();
}

void ActivityEditorMenu::reset() {
    lineEdit->setText(defaultLineEditText());
    if (!activity) {
        colorPicker->setRandomColor();
    }

    errorLabel->setText("");
    errorWidgetAction->setVisible(false);
}

void ActivityEditorMenu::deleteAndClose() {
    emit deleteActivity();
    close();
    reset();
}

void ActivityEditorMenu::colorPickerClicked() {
    if (activity)
        saveAndClose();
}

void ActivityEditorMenu::focus() {
    lineEdit->setFocus();
}
