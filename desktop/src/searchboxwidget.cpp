//
// Created by Dmitry Khrykin on 2019-12-25.
//

#include <QAction>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>
#include <QPalette>

#include "applicationsettings.h"
#include "searchboxwidget.h"

SearchBoxWidget::SearchBoxWidget(const QString &placeholder, QWidget *parent)
    : QWidget(parent) {
    using namespace ApplicationSettings;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFocusPolicy(Qt::ClickFocus);

    installEventFilter(this);

    lineEdit = new QLineEdit();
    lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
    lineEdit->setStyleSheet("QLineEdit {"
                            "border: transparent;"
                            "background: transparent;"
                            "}");
    lineEdit->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    lineEdit->setPlaceholderText(placeholder);
    lineEdit->installEventFilter(this);

    reloadPalette();

    connect(lineEdit, &QLineEdit::textEdited,
            this, &SearchBoxWidget::textEdited);

    auto *layout = new QHBoxLayout(this);

    layout->addWidget(lineEdit);
    layout->setContentsMargins(2 * defaultPadding + iconRect().width(),
                               5,
                               defaultPadding,
                               5);

    setContentsMargins(defaultPadding,
                       defaultPadding,
                       defaultPadding,
                       defaultPadding);

    cancelAction = new QAction("Cancel", this);
    cancelAction->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(cancelAction, &QAction::triggered, [this]() {
        if (!lineEdit->text().isEmpty()) {
            lineEdit->setText("");
            emit textEdited("");
        }

        lineEdit->clearFocus();
    });

    addAction(cancelAction);
}

void SearchBoxWidget::reloadPalette() const {
    auto palette = QApplication::palette();
    palette.setColor(QPalette::Text, textColorJustLighter());
    lineEdit->setPalette(palette);
}

QString SearchBoxWidget::text() {
    return lineEdit->text();
}

void SearchBoxWidget::setText(const QString &string) {
    lineEdit->setText(string);
}

void SearchBoxWidget::paintEvent(QPaintEvent *event) {
    auto painter = QPainter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);
    painter.setBrush(panelColor());

    painter.drawRoundedRect(contentsRect(), 8, 8);

    auto iconHeight = iconRect().height();

    painter.setFont(iconFont());
    painter.setPen(secondaryTextColor());
    painter.drawText(2 * ApplicationSettings::defaultPadding,
                     lineEdit->geometry().bottomLeft().y() - lineEdit->height() / 2 + iconHeight / 2 - 1, searchIconText());
}

QRect SearchBoxWidget::iconRect() const {
    auto metrics = QFontMetrics(iconFont());
    auto iconRect = metrics.boundingRect(searchIconText());
    return iconRect;
}

bool SearchBoxWidget::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::ApplicationPaletteChange) {
        reloadPalette();
    } else if (object == lineEdit) {
        if (event->type() == QEvent::FocusIn) {
            emit gotFocus();
            cancelAction->setEnabled(true);
        } else if (event->type() == QEvent::FocusOut) {
            emit lostFocus();
            cancelAction->setEnabled(false);
        } else if (event->type() == QEvent::KeyPress) {
            // Eat arrow keys an enter events
            auto keyEvent = dynamic_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Up ||
                keyEvent->key() == Qt::Key_Down ||
                keyEvent->key() == Qt::Key_Return ||
                keyEvent->key() == Qt::Key_Enter) {
                return true;
            }
        }
    }

    return false;
}

void SearchBoxWidget::focus() {
    lineEdit->setFocus();
}

void SearchBoxWidget::removeFocus() {
    lineEdit->clearFocus();
}
