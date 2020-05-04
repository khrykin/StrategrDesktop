//
// Created by Dmitry Khrykin on 2019-12-25.
//

#include <QLineEdit>
#include <QHBoxLayout>
#include <QPainter>
#include <QFontMetrics>
#include <QPalette>
#include <QAction>
#include <QFocusEvent>

#include "searchboxwidget.h"
#include "applicationsettings.h"

SearchBoxWidget::SearchBoxWidget(const QString &placeholder, QWidget *parent)
        : QWidget(parent) {

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFocusPolicy(Qt::ClickFocus);

    installEventFilter(this);

    lineEdit = new QLineEdit();
    lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
    lineEdit->setStyleSheet("QLineEdit {"
                            "border: transparent;"
                            "background: transparent;"
                            "}");
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setPlaceholderText(placeholder);
    lineEdit->installEventFilter(this);

    reloadPalette();

    connect(lineEdit,
            &QLineEdit::textEdited,
            this,
            &SearchBoxWidget::textEdited);

    auto *layout = new QHBoxLayout(this);

    layout->addWidget(lineEdit);
    layout->setContentsMargins(iconRect().width() + 3 * ApplicationSettings::defaultPadding,
                               ApplicationSettings::defaultPadding,
                               iconRect().width() + 3 * ApplicationSettings::defaultPadding,
                               ApplicationSettings::defaultPadding);

    cancelAction = new QAction("Cancel", this);
    cancelAction->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(cancelAction, &QAction::triggered,
            [this]() {
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
    painter.setFont(iconFont());

    auto iconHeight = iconRect().height();

    painter.setPen(textColorJustLighter());
    painter.drawText(2 * ApplicationSettings::defaultPadding,
                     lineEdit->geometry().bottomLeft().y()
                     - lineEdit->height() / 2
                     + iconHeight / 2 - 1, iconText);

    painter.setPen(Qt::NoPen);
    painter.setBrush(borderColor());
    auto borderRect = QRect(8,
                            height() - 1,
                            width() - 2 * ApplicationSettings::defaultPadding,
                            1);
    painter.drawRect(borderRect);
}

QRect SearchBoxWidget::iconRect() const {
    auto metrics = QFontMetrics(iconFont());
    auto iconRect = metrics.boundingRect(iconText);
    return iconRect;
}

QFont SearchBoxWidget::iconFont() const {
    QFont iconFont;
    iconFont.setFamily(ApplicationSettings::fontResourcePath);
    return iconFont;
}

bool SearchBoxWidget::eventFilter(QObject *object, QEvent *event) {
    if (object == this && event->type() == QEvent::ApplicationPaletteChange) {
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

