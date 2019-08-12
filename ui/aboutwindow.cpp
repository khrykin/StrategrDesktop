//
// Created by Dmitry Khrykin on 2019-08-12.
//

#include "aboutwindow.h"
#include "applicationicon.h"
#include "coloredlabel.h"
#include "colorprovider.h"
#include "applicationsettings.h"

AboutWindow::AboutWindow(QWidget *parent) : QDialog(parent) {
    setLayout(new QVBoxLayout());
    layout()->setSpacing(5);
    layout()->setContentsMargins(8, 8, 8, 8);
    setFixedWidth(300);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto *mainLabel = new QLabel("<b>Strategr Desktop</b>");
    mainLabel->setAlignment(Qt::AlignCenter);
    mainLabel->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Fixed);

    auto *versionLabel = new ColoredLabel(QString("Version %1")
                                                  .arg(ApplicationSettings::shortVersion));
    versionLabel->setDynamicColor(&ColorProvider::textColorJustLighter);
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
    versionLabel->setFontHeight(12);

    auto *copyLabel = new ColoredLabel(QString("Copyright © 2019 Dmitry Khrykin.\n"
                                               "All Rights Reserved."));
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
