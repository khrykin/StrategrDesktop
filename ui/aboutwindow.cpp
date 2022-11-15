//
// Created by Dmitry Khrykin on 2019-08-12.
//

#include <QDate>
#include <QDesktopServices>
#include <QUrl>

#include "aboutwindow.h"
#include "applicationicon.h"
#include "coloredlabel.h"
#include "colorprovider.h"
#include "applicationsettings.h"
#include "application.h"

class HyperLinkLabel : public ColoredLabel {
public:
    using ColoredLabel::ColoredLabel;

    void setHyperlink(const QString &hyperlink) {
        HyperLinkLabel::hyperlink = hyperlink;

        updateColor();
    }

private:
    QString hyperlink;

    enum {
        inactive,
        clicked,
        hovered
    } state = inactive;

    void updateColor() {
        auto color = dynamicColor();
        auto alpha = 1.0f;

        switch (state) {
            case clicked:
                alpha = 0.8;
                break;
            case hovered:
                alpha = 0.9;
                break;
            default:
                break;
        }

        color.setAlphaF(alpha);

        setDynamicColor([=] { return color; });
    }

    void mousePressEvent(QMouseEvent *event) override {
        state = clicked;
        updateColor();
    }

    void enterEvent(QEvent *event) override {
        state = hovered;
        setCursor(Qt::PointingHandCursor);
        updateColor();
    }

    void leaveEvent(QEvent *event) override {
        state = inactive;
        unsetCursor();
        updateColor();
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        state = inactive;
        updateColor();

        if (hyperlink.isEmpty())
            return;

        QDesktopServices::openUrl(QUrl(hyperlink));
    }
};

AboutWindow::AboutWindow(QWidget *parent) : QDialog(parent) {
    setAttribute(Qt::WA_DeleteOnClose);

    auto mainLayout = new QVBoxLayout();
    setLayout(mainLayout);
    layout()->setSpacing(8);
    layout()->setContentsMargins(8, 8, 8, 8);
    setFixedWidth(300);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto *mainLabel = new QLabel("<b>Strategr Desktop</b>");
    mainLabel->setAlignment(Qt::AlignCenter);
    mainLabel->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Fixed);

    auto versionString = QString("Version %1").arg(ApplicationSettings::version);
#ifdef Q_OS_WIN
    versionString += QString(" %1 bit").arg(ApplicationSettings::architectureString);
#endif

    auto *versionLabel = new ColoredLabel(versionString);
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
    copyLabel->setFontHeight(12);

    auto iconLabel = new QLabel();
    iconLabel->setPixmap(ApplicationIcon::defaultIcon());
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Fixed);

    auto *githubLabel = new HyperLinkLabel();
    githubLabel->setDynamicColor(&ColorProvider::textColorJustLighter);
    githubLabel->setText("Star on GitHub");
    githubLabel->setFontHeight(12);
    githubLabel->setAlignment(Qt::AlignRight);
    githubLabel->setSizePolicy(QSizePolicy::Expanding,
                               QSizePolicy::Fixed);
    githubLabel->setHyperlink(QStringLiteral("https://github.com/%1").arg(ApplicationSettings::repoName));

    auto *dotLabel = new ColoredLabel();
    dotLabel->setDynamicColor(&ColorProvider::textColorJustLighter);
    dotLabel->setText("·");
    dotLabel->setFontHeight(12);
    dotLabel->setAlignment(Qt::AlignCenter);
    dotLabel->setSizePolicy(QSizePolicy::Fixed,
                            QSizePolicy::Fixed);

    auto *issueLabel = new HyperLinkLabel();
    issueLabel->setDynamicColor(&ColorProvider::textColorJustLighter);
    issueLabel->setText("Report an issue");
    issueLabel->setFontHeight(12);
    issueLabel->setAlignment(Qt::AlignLeft);
    issueLabel->setSizePolicy(QSizePolicy::Expanding,
                              QSizePolicy::Fixed);
    issueLabel->setHyperlink(QStringLiteral("https://github.com/%1/issues/new").arg(ApplicationSettings::repoName));


    layout()->addWidget(iconLabel);
    layout()->addWidget(mainLabel);
    layout()->addWidget(versionLabel);
    layout()->addWidget(copyLabel);

    auto githubLayout = new QHBoxLayout();
    githubLayout->setSpacing(5);
    githubLayout->setContentsMargins(0, 0, 0, 0);

    githubLayout->addWidget(githubLabel);
    githubLayout->addWidget(dotLabel);
    githubLayout->addWidget(issueLabel);

    mainLayout->addLayout(githubLayout);

    layout()->addWidget(new QWidget());

    setMinimumHeight(sizeHint().height());
    setMaximumHeight(sizeHint().height());
}

AboutWindow::~AboutWindow() {
    Application::aboutWindow = nullptr;
}
