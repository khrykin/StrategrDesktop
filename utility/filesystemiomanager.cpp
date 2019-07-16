#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>

#include "filesystemiomanager.h"
#include "jsonserializer.h"
#include "notifierbackend.h"
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
            destinationDir(),
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
                         JSONSerializer(strategy).write());
    QMessageBox msgBox(window);
    msgBox.setText("Current strategy has been saved as your default.");
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.exec();
}

std::unique_ptr<Strategy>
FileSystemIOManager::read(const QString &readFilepath) {
    if (readFilepath.isEmpty()) {
        return nullptr;
    }

    filepath = readFilepath;

    QFile file(filepath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        showCantOpenDialog(file);
        return nullptr;
    };

    QTextStream in(&file);
    auto strategy = JSONSerializer::read(in.readAll());

    file.close();

    if (!strategy) {
        showCantOpenDialog(file);
    } else {
        updateLastOpened();
        setIsSaved(true);
    }

    return strategy;
}

std::unique_ptr<Strategy>
FileSystemIOManager::lastOpened() {
    QSettings settings;
    if (!settings.value(Settings::lastOpenedStrategyKey).isNull()) {
        auto lastFilepath = settings
                .value(Settings::lastOpenedStrategyKey)
                .toString();

        return read(lastFilepath);
    }

    return nullptr;
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

std::unique_ptr<Strategy>
FileSystemIOManager::openDefaultStrategy() {
    if (defaultStrategyIsSet()) {
        auto defaultStrategyString = QSettings()
                .value(Settings::defaultStrategyKey)
                .toString();

        auto defaultStrategy = JSONSerializer::read(defaultStrategyString);

        resetFilepath();

        return defaultStrategy
               ? std::move(defaultStrategy)
               : std::make_unique<Strategy>();
    }

    resetFilepath();

    return std::make_unique<Strategy>();
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
    auto json = JSONSerializer(strategy).write();
    QSaveFile file(filepath);

    if (file.open(QIODevice::WriteOnly
                  | QIODevice::Truncate
                  | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << json << endl;

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

void FileSystemIOManager::showCantOpenDialog(const QFile &file) {
    QMessageBox::warning(window, QObject::tr("Open Strategy"),
                         QObject::tr("Can't read file %1:\n%2.")
                                 .arg(filepath)
                                 .arg(file.errorString()));
}


void FileSystemIOManager::showCantSaveDialog(const QSaveFile &file) {
    QMessageBox::warning(window, QObject::tr("Save Strategy"),
                         QObject::tr("Can't write to file %1:\n%2.")
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

