#include "navbar.h"
#include <QLayout>
#include <QLabel>
#include <QVariant>
#include <QStyleOption>
#include <QPainter>

Navbar::Navbar(QWidget *parent) : QWidget(parent)
{
    setLayout(new QHBoxLayout());
    layout()->setSpacing(0);
    layout()->setMargin(0);
    setFixedHeight(50);
    setStyleSheet("Navbar {"
//                  "border-bottom: 1px solid blue;"
                  "}");

    _titleLabel = new QLabel(_title);
    _titleLabel->setAlignment(Qt::AlignCenter);
    _titleLabel->setStyleSheet("font-size: 18px;");

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

    layout()->addWidget(_leftButton);
    layout()->addWidget(_titleLabel);

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

    layout()->addWidget(_rightButton);
}

QString Navbar::title() const
{
    return _title;
}

void Navbar::setTitle(const QString &title)
{
    _title = title;
    _titleLabel->setText(title);
}

void Navbar::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
