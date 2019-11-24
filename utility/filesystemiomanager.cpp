#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>

#include "filesystemiomanager.h"
#include "ui/mainwindow.h"
#include "alert.h"

FileSystemIOManager::FileSystemIOManager(QWidget *parent) : window(parent) {}

QString FileSystemIOManager::getOpenFileName() {
    auto openFilepath = QFileDialog::getOpenFileName(
            window, QObject::tr("Open Strategy"), destinationDir(), searchPattern);

    QSettings().setValue(Settings::lastOpenedDirectoryKey, openFilepath);
    return openFilepath;
}

void FileSystemIOManager::save(const Strategy &strategy) {
    if (filepath.isEmpty()) {
        saveAs(strategy);
        return;
    }

    write(strategy);
}

void FileSystemIOManager::saveAs(const Strategy &strategy) {
    auto saveAsFilepath = QFileDialog::getSaveFileName(
            window,
            QObject::tr("Save Strategy As"),
            destinationDir() + "/" + fileInfo().fileName(),
            searchPattern);

    QSettings().setValue(Settings::lastOpenedDirectoryKey, saveAsFilepath);

    if (saveAsFilepath.isEmpty()) {
        return;
    }

    filepath = saveAsFilepath;
    QSettings().setValue(Settings::lastOpenedDirectoryKey,
                         fileInfo().absolutePath());
    write(strategy);
}

void FileSystemIOManager::saveAsDefault(const Strategy &strategy) {
    QSettings().setValue(Settings::defaultStrategyKey,
                         QString::fromStdString(strategy.toJsonString()));
    QMessageBox msgBox(window);
    msgBox.setText("Current strategy has been saved as your default.");
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.exec();
}

std::optional<Strategy>
FileSystemIOManager::read(const QString &readFilepath) {
    if (readFilepath.isEmpty()) {
        return std::nullopt;
    }

    QFile file(readFilepath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        showCantOpenDialog(file, readFilepath, file.errorString());
        return std::nullopt;
    }

    QTextStream in(&file);
    auto strategy = Strategy::fromJsonString(in.readAll().toStdString());

    file.close();

    if (!strategy) {
        auto errorMessage = QObject::tr("The file is corrupt or has a wrong format");
        showCantOpenDialog(file, readFilepath, errorMessage);
    } else {
        filepath = readFilepath;
        updateLastOpened();
        setIsSaved(true);
    }

    return strategy;
}

std::optional<QString>
FileSystemIOManager::lastOpenedFilePath() {
    QSettings settings;
    if (!settings.value(Settings::lastOpenedStrategyKey).isNull()) {
        auto lastFilepath = settings
                .value(Settings::lastOpenedStrategyKey)
                .toString();

        return lastFilepath;
    }

    return std::nullopt;
}

void FileSystemIOManager::resetFilepath() {
    filepath = "";
}

void FileSystemIOManager::clearRecent() {
    QSettings().setValue(Settings::recentFilesKey, QStringList());
}

bool FileSystemIOManager::isSaved() const {
    return _isSaved;
}

void FileSystemIOManager::setIsSaved(bool isSaved) {
    _isSaved = isSaved;
}

Strategy
FileSystemIOManager::openDefaultStrategy() {
    if (defaultStrategyIsSet()) {
        auto defaultStrategyString = QSettings()
                .value(Settings::defaultStrategyKey)
                .toString()
                .toStdString();

        auto defaultStrategy = Strategy::fromJsonString(defaultStrategyString);

        resetFilepath();

        return defaultStrategy.value_or(Strategy());
    }

    resetFilepath();

    return Strategy();
}

bool FileSystemIOManager::defaultStrategyIsSet() {
    return !QSettings().value(Settings::defaultStrategyKey).isNull();
}

QFileInfo FileSystemIOManager::fileInfo() { return QFileInfo(filepath); }

QStringList FileSystemIOManager::recentPaths() {
    auto recentFilesSetting = QSettings().value(Settings::recentFilesKey);
    return recentFilesSetting.toStringList();
}

QStringList FileSystemIOManager::recentFileNames() {
    QStringList result;

    for (auto &path : recentPaths()) {
        result.append(QFileInfo(path).baseName());
    }

    return result;
}

void FileSystemIOManager::write(const Strategy &strategy) {
    auto json = strategy.toJsonString();
    QSaveFile file(filepath);

    if (file.open(QIODevice::WriteOnly
                  | QIODevice::Truncate
                  | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << QString::fromStdString(json) << endl;

        if (!file.commit()) {
            showCantSaveDialog(file);
        }

        updateLastOpened();
        setIsSaved(true);
    } else {
        showCantSaveDialog(file);
    }
}

QString FileSystemIOManager::destinationDir() {
    QSettings settings;
    if (!settings.value(Settings::lastOpenedDirectoryKey).isNull()) {
        auto lastOpenedDirectory =
                settings.value(Settings::lastOpenedDirectoryKey).toString();

        return lastOpenedDirectory;
    }

    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
}

void FileSystemIOManager::updateLastOpened() {
    auto absoluteFilePath = fileInfo().absoluteFilePath();

    QSettings settings;
    settings.setValue(Settings::lastOpenedStrategyKey, absoluteFilePath);
    settings.setValue(Settings::lastOpenedDirectoryKey,
                      fileInfo().absolutePath());

    auto files = recentPaths();
    files.removeAll(absoluteFilePath);
    files.prepend(absoluteFilePath);

    while (files.size() > Settings::numberOfRecent) {
        files.removeLast();
    }

    settings.setValue(Settings::recentFilesKey, files);
            foreach (QWidget *widget, QApplication::topLevelWidgets()) {
            auto *mainWindow = qobject_cast<MainWindow *>(widget);
            if (mainWindow && mainWindow->menu()) {
                mainWindow->menu()->updateRecentFilesActions();
            }
        }
}

void FileSystemIOManager::showCantOpenDialog(const QFile &file,
                                             const QString &path,
                                             const QString &errorMessage) {
    Alert::showWarning(window,
                       QObject::tr("Strategy can't be opened"),
                       QObject::tr("Can't parse file \"%1\":\n\n%2.")
                               .arg(path)
                               .arg(errorMessage));
}


void FileSystemIOManager::showCantSaveDialog(const QSaveFile &file) {
    Alert::showWarning(window,
                       QObject::tr("Strategy can't be saved"),
                       QObject::tr("Can't serialize to file \"%1\":\n\n%2.")
                               .arg(filepath)
                               .arg(file.errorString()));
}

bool FileSystemIOManager::askIfWantToDiscardOrLeaveCurrent(const Strategy &strategy) {
    auto returnValue = showAreYouSureDialog();

    if (returnValue == QMessageBox::Save) {
        save(strategy);
    } else if (returnValue == QMessageBox::Cancel) {
        return WantToLeaveAsIs;
    }

    return WantToDiscard;
}

int FileSystemIOManager::showAreYouSureDialog() {
    return Alert::showAskToSave("Document "
                                + fileInfo().fileName()
                                + " has been modified.",
                                "Do you want to save your changes?");
}

void FileSystemIOManager::setWindow(QWidget *newWindow) {
    window = newWindow;
}

