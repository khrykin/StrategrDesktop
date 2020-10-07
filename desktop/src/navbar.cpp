#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QStyleOption>
#include <QVariant>

#include "applicationsettings.h"
#include "navbar.h"

#ifdef HIDE_NAVBAR

Navbar::Navbar(QWidget *parent) : QWidget(parent) {
    setFixedHeight(0);
}

void Navbar::setTitle(const QString &title) {}

QWidget *Navbar::leftButton() const {
    return nullptr;
}

QWidget *Navbar::rightButton() const {
    return nullptr;
}

QLabel *Navbar::titleLabel() const {
    return nullptr;
}

#else

Navbar::Navbar(QWidget *parent) : QWidget(parent) {
    setupLayout();
    setupWidgets();
    addWidgetsToLayout();
}

QString Navbar::title() const { return _title; }

void Navbar::setTitle(const QString &title) {
    _title = title;
    _titleLabel->setText(title);
}

QWidget *Navbar::leftButton() const {
    return _leftButton;
}

QWidget *Navbar::rightButton() const {
    return _rightButton;
}

QLabel *Navbar::titleLabel() const {
    return _titleLabel;
}

void Navbar::paintEvent(QPaintEvent *) {
    auto painter = QPainter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(baseColor());
    painter.drawRect(0, 0, width(), height());
    //    painter.setBrush(borderColor());
    //    painter.drawRect(0, height() - 1, width(), 1);
}

void Navbar::setupWidgets() {
    setupTitleLabel();
    setupLeftButton();
    setupRightButton();
}

void Navbar::setupLayout() {
    setLayout(new QHBoxLayout());
    layout()->setSpacing(0);
    layout()->setContentsMargins(0, 0, 0, 0);
    setFixedHeight(40);
}

void Navbar::setupLeftButton() {
    _leftButton = new IconWidget("", this);
    _leftButton->setFixedWidth(64);
    _leftButton->setColor("#094FD1");
}

void Navbar::setupRightButton() {
    _rightButton = new IconWidget("", this);
    _rightButton->setFixedWidth(64);
    _rightButton->setColor("#094FD1");
}

void Navbar::setupTitleLabel() {
    _titleLabel = new QLabel(_title);
    _titleLabel->setAlignment(Qt::AlignCenter);
    _titleLabel->setStyleSheet("font-weight: bold;"
                               "font-size: 14px;");
}

void Navbar::addWidgetsToLayout() {
    layout()->addWidget(_leftButton);
    layout()->addWidget(_titleLabel);
    layout()->addWidget(_rightButton);
}

#endif