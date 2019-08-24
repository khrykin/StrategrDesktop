//
// Created by Dmitry Khrykin on 2019-08-22.
//

#ifndef STRATEGR_UPDATEDIALOG_H
#define STRATEGR_UPDATEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QBoxLayout>
#include <QProgressBar>

class UpdateDialog : public QDialog {
public:
    enum class State {
        Checking,
        UpdateAvaliable,
        UpdateUnavaliable,
        DownloadingUpdate
    };

    explicit UpdateDialog(QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout = nullptr;
    QHBoxLayout *progressLayout = nullptr;
    QHBoxLayout *buttonsLayout = nullptr;

    QLabel *progressLabel = nullptr;
    QLabel *infoLabel = nullptr;

    QPushButton *downloadButton = nullptr;
    QPushButton *okButton = nullptr;
    QPushButton *cancelButton = nullptr;

    QProgressBar *progressBar = nullptr;

    State state = State::Checking;
    void finish();
    void handleHasUpdate(const QString &version) const;
    void handleNoUpdate(const QString &version) const;
    void performUpdate();
    void layoutChildWidgets();
};

#endif //STRATEGR_UPDATEDIALOG_H
