#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>
#include <QDir>

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

void FileSystemIOManager::save(const stg::strategy &strategy) {
    if (filepath.isEmpty()) {
        saveAs(strategy);
        return;
    }

    write(strategy);
}

void FileSystemIOManager::saveAs(const stg::strategy &strategy) {
    auto filePath = QDir(destinationDir()).absoluteFilePath(fileInfo().fileName());
    auto saveAsFilepath = QFileDialog::getSaveFileName(
            window,
            QObject::tr("Save Strategy As"),
            filePath,
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

void FileSystemIOManager::saveAsDefault(const stg::strategy &strategy) {
    QSettings().setValue(Settings::defaultStrategyKey,
                         QString::fromStdString(strategy.to_json_string()));
    QMessageBox msgBox(window);
    msgBox.setText("Current strategy has been saved as your default.");
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.exec();
}

std::unique_ptr<stg::strategy>
FileSystemIOManager::read(const QString &readFilepath) {
    if (readFilepath.isEmpty()) {
        return nullptr;
    }

    QFile file(readFilepath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        showCantOpenDialog(file, readFilepath, file.errorString());
        return nullptr;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    auto strategy = stg::strategy::from_json_string(in.readAll().toStdString());

    file.close();

    if (strategy) {
        filepath = readFilepath;
        updateLastOpened();
        setIsSaved(true);
        return strategy;
    } else {
        auto errorMessage = QObject::tr("The file is corrupt or has a wrong format");
        showCantOpenDialog(file, readFilepath, errorMessage);
    }

    return nullptr;
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

std::unique_ptr<stg::strategy>
FileSystemIOManager::openDefault() {
    if (defaultStrategyIsSet()) {
        auto defaultStrategyString = QSettings()
                .value(Settings::defaultStrategyKey)
                .toString()
                .toStdString();

        auto defaultStrategy = stg::strategy::from_json_string(defaultStrategyString);

        resetFilepath();

        if (!defaultStrategy) {
            return std::make_unique<stg::strategy>();
        }

        return defaultStrategy;
    }

    resetFilepath();

    return std::make_unique<stg::strategy>();
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

void FileSystemIOManager::write(const stg::strategy &strategy) {
    auto json = strategy.to_json_string();
    QSaveFile file(filepath);

    if (file.open(QIODevice::WriteOnly
                  | QIODevice::Truncate
                  | QIODevice::Text)) {

        QTextStream stream(&file);
        stream.setCodec("UTF-8");
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

bool FileSystemIOManager::askIfWantToDiscardOrLeaveCurrent(const stg::strategy &strategy) {
    auto returnValue = showAreYouSureDialog();

    if (returnValue == QMessageBox::Save) {
        save(strategy);
    } else if (returnValue == QMessageBox::Cancel) {
        return WantToLeaveAsIs;
    }

    return WantToDiscard;
}

int FileSystemIOManager::showAreYouSureDialog() {
    const auto title = fileInfo().fileName().isEmpty()
                       ? "New document hasn't been saved"
                       : "Document "
                         + fileInfo().fileName()
                         + " has been modified";

    return Alert::showAskToSave(title,
                                "Do you want to save your changes?");
}

void FileSystemIOManager::setWindow(QWidget *newWindow) {
    window = newWindow;
}

std::unique_ptr<stg::strategy> FileSystemIOManager::openLastOrDefault() {
    auto lastOpenedPath = FileSystemIOManager::lastOpenedFilePath();
    if (lastOpenedPath) {
        auto lastOpened = read(*lastOpenedPath);
        if (lastOpened) {
            return lastOpened;
        }
    }

    return openDefault();
}

