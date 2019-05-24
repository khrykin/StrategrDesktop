#include "navbar.h"
#include <QLayout>
#include <QLabel>
#include <QVariant>

Navbar::Navbar(QWidget *parent) : QWidget(parent)
{
    setLayout(new QVBoxLayout());
    layout()->setSpacing(0);
    layout()->setMargin(0);

    navbar = new QWidget();
    navbar->setLayout(new QHBoxLayout());
    navbar->layout()->setSpacing(0);
    navbar->layout()->setMargin(0);
    navbar->setFixedHeight(50);


    navbar->setProperty("navbar", true);
    navbar->setStyleSheet("[navbar] {"
                          "border-bottom: 1px solid black;"
                          "}");

    layout()->addWidget(navbar);

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

    navbar->layout()->addWidget(_leftButton);
    navbar->layout()->addWidget(_titleLabel);

    layout()->addWidget(navbar);

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

    navbar->layout()->addWidget(_rightButton);
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
