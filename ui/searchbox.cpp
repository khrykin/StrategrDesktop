//
// Created by Dmitry Khrykin on 2019-12-25.
//

#include <QLineEdit>
#include <QHBoxLayout>
#include <QPainter>
#include <QFontMetrics>

#include "searchbox.h"
#include "applicationsettings.h"

SearchBox::SearchBox(const QString &placeholder, QWidget *parent)
        : QWidget(parent) {

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFocusPolicy(Qt::ClickFocus);

    lineEdit = new QLineEdit();
    lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
    lineEdit->setStyleSheet("QLineEdit {"
                            "border: transparent;"
                            "background: transparent;"
                            "}");
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setPlaceholderText(placeholder);

    connect(lineEdit,
            &QLineEdit::textEdited,
            this,
            &SearchBox::textEdited);

    auto layout = new QHBoxLayout(this);

    layout->addWidget(lineEdit);
    layout->setContentsMargins(iconRect().width() + 3 * ApplicationSettings::defaultPadding,
                               ApplicationSettings::defaultPadding,
                               iconRect().width() + 3 * ApplicationSettings::defaultPadding,
                               ApplicationSettings::defaultPadding);
}

QString SearchBox::text() {
    return lineEdit->text();
}

void SearchBox::setText(const QString &string) {
    lineEdit->setText(string);
}

void SearchBox::paintEvent(QPaintEvent *event) {
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

QRect SearchBox::iconRect() const {
    auto metrics = QFontMetrics(iconFont());
    auto iconRect = metrics.boundingRect(iconText);
    return iconRect;
}

QFont SearchBox::iconFont() const {
    QFont iconFont;
    iconFont.setFamily(ApplicationSettings::fontResourcePath);
//    iconFont.setPixelSize(font().pixelSize());
    return iconFont;
}

