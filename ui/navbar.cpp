#include "navbar.h"
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QStyleOption>
#include <QVariant>
#include "applicationsettings.h"

#ifdef HIDE_NAVBAR

Navbar::Navbar(QWidget *parent) : QWidget(parent) {
    setFixedHeight(0);
}

void Navbar::setTitle(const QString &title) {}

QPushButton *Navbar::leftButton() const {
    return nullptr;
}

QPushButton *Navbar::rightButton() const {
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

QPushButton *Navbar::leftButton() const {
    return _leftButton;
}

QPushButton *Navbar::rightButton() const {
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
    setFixedHeight(45);
}

void Navbar::setupLeftButton() {
    _leftButton = new QPushButton("");
    _leftButton->setProperty("navButton", true);
    _leftButton->setFixedWidth(70);
    _leftButton->setStyleSheet("[navButton] {"
                               "background: transparent;"
                               "font-size: 14px;"
                               "color: #357EDD;"
                               "}"
                               "[navButton]:hover {"
                               "color: #00449E;"
                               "}");
}

void Navbar::setupTitleLabel() {
    _titleLabel = new QLabel(_title);
    _titleLabel->setAlignment(Qt::AlignCenter);
    _titleLabel->setStyleSheet("font-size: 18px;");
}

void Navbar::setupRightButton() {
    _rightButton = new QPushButton("");
    _rightButton->setProperty("navButton", true);
    _rightButton->setFixedWidth(70);
    _rightButton->setStyleSheet("[navButton] {"
                                "background: transparent;"
                                "font-size: 14px;"
                                "color: #357EDD;"
                                "}"
                                "[navButton]:hover {"
                                "color: #00449E;"
                                "}");
}

void Navbar::addWidgetsToLayout() {
    layout()->addWidget(_leftButton);
    layout()->addWidget(_titleLabel);
    layout()->addWidget(_rightButton);
}

#endif