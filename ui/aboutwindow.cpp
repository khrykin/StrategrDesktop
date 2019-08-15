//
// Created by Dmitry Khrykin on 2019-08-12.
//
#include <QDate>

#include "aboutwindow.h"
#include "applicationicon.h"
#include "coloredlabel.h"
#include "colorprovider.h"
#include "applicationsettings.h"

AboutWindow::AboutWindow(QWidget *parent) : QDialog(parent) {
    setLayout(new QVBoxLayout());
    layout()->setSpacing(8);
    layout()->setContentsMargins(8, 8, 8, 8);
    setFixedWidth(300);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto *mainLabel = new QLabel("<b>Strategr Desktop</b>");
    mainLabel->setAlignment(Qt::AlignCenter);
    mainLabel->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Fixed);

    auto *versionLabel = new ColoredLabel(QString("Version %1")
                                                  .arg(ApplicationSettings::version));
    versionLabel->setDynamicColor(&ColorProvider::textColorJustLighter);
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
    versionLabel->setFontHeight(12);

    QString years = "2019";
    auto currentYear = QDate::currentDate().year();
    if (currentYear != 2019) {
        years += "-" + QString::number(currentYear);
    }

    auto *copyLabel = new ColoredLabel(QString("Copyright © %1 %2.\n"
                                               "All Rights Reserved.")
                                               .arg(years)
                                               .arg(QApplication::organizationName()));
    copyLabel->setDynamicColor(&ColorProvider::textColorJustLighter);
    copyLabel->setAlignment(Qt::AlignCenter);
    copyLabel->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Fixed);
    copyLabel->setFontHeight(12);

    auto iconLabel = new QLabel();
    iconLabel->setPixmap(ApplicationIcon::defaultIcon());
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Fixed);

    layout()->addWidget(iconLabel);
    layout()->addWidget(mainLabel);
    layout()->addWidget(versionLabel);
    layout()->addWidget(copyLabel);

    layout()->addWidget(new QWidget());

    setMaximumHeight(sizeHint().height());
}
