#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>

#include "application.h"
#include "filesystemiomanager.h"
#include "mainwindow.h"
#include "applicationmenu.h"
#include "alert.h"

FileSystemIOManager::FileSystemIOManager(QWidget *parent) : window(parent) {}

QString FileSystemIOManager::getOpenFileName() {
    auto openFilepath = QFileDialog::getOpenFileName(window,
                                                     QObject::tr("Open Strategy"), destinationDir(),
                                                     searchPattern);

    Application::currentSettings()
            .setValue(Settings::lastOpenedDirectoryKey, openFilepath);

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
    auto saveAsFilepath = QFileDialog::getSaveFileName(window,
                                                       QObject::tr("Save Strategy As"),
                                                       filePath,
                                                       searchPattern);

    Application::currentSettings()
            .setValue(Settings::lastOpenedDirectoryKey, saveAsFilepath);

    if (saveAsFilepath.isEmpty()) {
        return;
    }

    filepath = saveAsFilepath;
    _isSaved = true;

    Application::currentSettings()
            .setValue(Settings::lastOpenedDirectoryKey, fileInfo().absolutePath());

    write(strategy);
}

void FileSystemIOManager::saveAsDefault(const stg::strategy &strategy) {
    strategy.save_as_default();

    auto msgBox = QMessageBox(window);
    msgBox.setText("Current strategy has been saved as your default.");
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.exec();
}

std::unique_ptr<stg::strategy> FileSystemIOManager::read(const QString &readFilepath) {
    if (readFilepath.isEmpty())
        return nullptr;

    try {
        auto strategy = stg::strategy::from_file(readFilepath.toStdString());
        if (strategy) {
            filepath = readFilepath;
            updateLastOpened();
            setIsSaved(true);

            return strategy;
        } else {
            auto errorMessage = "The file is corrupt or has a wrong format";
            showCantOpenDialog(readFilepath, errorMessage);
        }
    } catch (const stg::strategy::file_read_exception &) {
        auto errorMessage = "The file doesn't exist";
        showCantOpenDialog(readFilepath, errorMessage);
    }

    return nullptr;
}

std::optional<QString> FileSystemIOManager::lastOpenedFilePath() {
    auto &settings = Application::currentSettings();
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
    Application::currentSettings()
            .setValue(Settings::recentFilesKey, QStringList());
}

bool FileSystemIOManager::isSaved() const {
    return _isSaved;
}

void FileSystemIOManager::setIsSaved(bool isSaved) {
    _isSaved = isSaved;
}

stg::strategy
FileSystemIOManager::openDefault() {
    resetFilepath();

    return stg::strategy::from_default();
}


QFileInfo FileSystemIOManager::fileInfo() { return QFileInfo(filepath); }

QStringList FileSystemIOManager::recentPaths() {
    auto recentFilesSetting = Application::currentSettings().value(Settings::recentFilesKey);
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
    try {
        strategy.write_to_file(filepath.toStdString());

        updateLastOpened();
        setIsSaved(true);
    } catch (const stg::strategy::file_write_exception &) {
        showCantSaveDialog();
    }
}

QString FileSystemIOManager::destinationDir() {
    auto &settings = Application::currentSettings();
    if (!settings.value(Settings::lastOpenedDirectoryKey).isNull()) {
        auto lastOpenedDirectory =
                settings.value(Settings::lastOpenedDirectoryKey).toString();

        return lastOpenedDirectory;
    }

    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
}

void FileSystemIOManager::updateLastOpened() {
    auto absoluteFilePath = fileInfo().absoluteFilePath();

    auto &settings = Application::currentSettings();
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

void FileSystemIOManager::showCantOpenDialog(const QString &path,
                                             const QString &errorMessage) {
    Alert::showWarning(window,
                       QObject::tr("Strategy can't be opened"),
                       QObject::tr("Can't read file \"%1\":\n\n%2.")
                               .arg(path)
                               .arg(errorMessage));
}


void FileSystemIOManager::showCantSaveDialog() {
    Alert::showWarning(window,
                       QObject::tr("Strategy can't be saved"),
                       QObject::tr("Can't save to the \"%1\"")
                               .arg(filepath));
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

stg::strategy FileSystemIOManager::openLastOrDefault() {
    auto lastOpenedPath = FileSystemIOManager::lastOpenedFilePath();
    if (lastOpenedPath) {
        auto lastOpened = read(*lastOpenedPath);
        if (lastOpened)
            return *lastOpened;

    }

    return openDefault();
}

stg::strategy FileSystemIOManager::openFromPathOrDefault(const QString &path) {
    auto strategy = read(path);

    if (!strategy)
        return openDefault();

    return *strategy;
}

