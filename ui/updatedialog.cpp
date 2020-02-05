//
// Created by Dmitry Khrykin on 2019-08-22.
//

#include <optional>

#include "updatedialog.h"
#include "application.h"

UpdateDialog::UpdateDialog(QWidget *parent) : QDialog(parent) {
    setFixedSize(450, 135);
    setWindowTitle(tr("Update Strategr"));

    layoutChildWidgets();

    Application::updater().check([=](bool hasUpdate,
                                     const QString &version,
                                     const std::optional<QString> &error) {
        if (error) {
            // Application::updater() will show it's alert,
            // we just close this dialog
            finish();
            return;
        }

        if (hasUpdate) {
            handleHasUpdate(version);
        } else {
            handleNoUpdate(version);
        }
    });
}

void UpdateDialog::layoutChildWidgets() {
    mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(16, 16, 16, 14);
    setLayout(mainLayout);

    progressLabel = new QLabel();
    progressLabel->setFixedWidth(30);
    progressLabel->hide();

    progressLayout = new QHBoxLayout(this);
    progressLayout->setContentsMargins(0, 0, 0, 0);
    progressLayout->setSpacing(8);

    progressBar = new QProgressBar();
    progressBar->setRange(0, 0);


    infoLabel = new QLabel("Checking for updates...");

    progressLayout->addWidget(progressBar);
    progressLayout->addWidget(progressLabel);

    mainLayout->addStretch();
    mainLayout->addWidget(infoLabel);
    mainLayout->addLayout(progressLayout);
    mainLayout->addStretch();

    buttonsLayout = new QHBoxLayout();
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(8);

    downloadButton = new QPushButton(tr("Download"));
    downloadButton->setDefault(true);
    downloadButton->setEnabled(false);

    okButton = new QPushButton(tr("OK"));
    okButton->setDefault(true);
    okButton->hide();

    cancelButton = new QPushButton(tr("Cancel"));

    buttonsLayout->addStretch();
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(downloadButton);
    buttonsLayout->addWidget(okButton);

    mainLayout->addLayout(buttonsLayout);

    connect(cancelButton,
            &QPushButton::clicked,
            this,
            &UpdateDialog::finish);

    connect(okButton,
            &QPushButton::clicked,
            this,
            &UpdateDialog::finish);

    connect(downloadButton,
            &QPushButton::clicked,
            this,
            &UpdateDialog::performUpdate);
}

void UpdateDialog::performUpdate() {
    infoLabel->setText("Downloading an update...");
    progressBar->show();
    progressBar->setRange(0, 100);
    progressBar->setTextVisible(true);
    downloadButton->setEnabled(false);
    progressLabel->setText("0 %");
    progressLabel->show();

    Application::updater().performUpdate([=](double progress,
                                             const std::optional<QString> &error) {
        if (error) {
            finish();
            return;
        }

        auto percentage = static_cast<int>(progress * 100);
        progressBar->setValue(percentage);
        progressLabel->setText(QString("%1 %")
                                       .arg(percentage));

        if (progress == 1) {
            finish();
        }
    });
}

void UpdateDialog::handleNoUpdate(const QString &version) const {
    infoLabel->setText(QString(tr("Strategr has_only up to date."))
                               .arg(version));

    downloadButton->hide();
    cancelButton->hide();

    okButton->show();
    progressBar->hide();
}

void UpdateDialog::handleHasUpdate(const QString &version) const {
    infoLabel->setText(QString(tr("<b>New update's avaliable (%1)</b>"
                                  "<br/>"
                                  "Download and install the new version?"))
                               .arg(version));

    downloadButton->setEnabled(true);
    progressBar->hide();
}

void UpdateDialog::finish() {
    this->close();
    this->deleteLater();
    Application::updateDialog = nullptr;
}
