#ifndef FILESYSTEMIOMANAGER_H
#define FILESYSTEMIOMANAGER_H

#include <QFileInfo>
#include <QSaveFile>
#include <QObject>

#include "models/strategy.h"

class FileSystemIOManager {
public:
    static const bool WantToDiscard = false;
    static const bool WantToLeaveAsIs = true;

    static bool defaultStrategyIsSet();
    static QStringList recentPaths();
    static QStringList recentFileNames();
    static void clearRecent();

    explicit FileSystemIOManager(QWidget *parent);

    bool isSaved() const;
    void setIsSaved(bool isSaved);

    std::unique_ptr<Strategy> open();

    void save(const Strategy &strategy);
    void saveAs(const Strategy &strategy);
    void saveAsDefault(const Strategy &strategy);

    std::unique_ptr<Strategy> read(const QString &readFilepath);
    std::unique_ptr<Strategy> lastOpened();

    std::unique_ptr<Strategy> openDefaultStrategy();

    void resetFilepath();
    bool askIfWantToDiscardOrLeaveCurrent(const Strategy &strategy);

    QFileInfo fileInfo();

    struct Settings {
        static constexpr auto lastOpenedDirectoryKey = "lastOpenedDirectory";
        static constexpr auto lastOpenedStrategyKey = "lastOpenedStrategy";
        static constexpr auto recentFilesKey = "recentFiles";
        static constexpr auto defaultStrategyKey = "defaultStrategy";
        static constexpr int numberOfRecent = 5;
    };

private:
    /// True if current state is saved in a file
    bool _isSaved = true;

    /// Holds opened file's path
    QString filepath;

    QWidget *window;

    static constexpr auto searchPattern = "Strategy files (*.stg)";

    static QString destinationDir();
    void write(const Strategy &strategy);
    void updateLastOpened();
    int showAreYouSureDialog();
    void showCantOpenDialog(const QFile &file);
    void showCantSaveDialog(const QSaveFile &file);

};

#endif // FILESYSTEMIOMANAGER_H
