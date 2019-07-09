#ifndef FILESYSTEMIOMANAGER_H
#define FILESYSTEMIOMANAGER_H

#include <QFileInfo>
#include <QSaveFile>
#include <QObject>

#include "models/strategy.h"

class FileSystemIOManager {
public:
    static const bool WantToDiscard = true;
    static const bool WantToLeaveAsIs = false;

    static bool defaultStrategyIsSet();
    static QStringList recentPaths();
    static QStringList recentFileNames();
    static void clearRecent();

    explicit FileSystemIOManager(QWidget *parent);

    bool isSaved() const;
    void setIsSaved(bool isSaved);

    std::optional<Strategy> open();

    void save(const Strategy &strategy);
    void saveAs(const Strategy &strategy);
    void saveAsDefault(const Strategy &strategy);

    std::optional<Strategy> read(const QString &readFilepath);
    std::optional<Strategy> lastOpened();

    Strategy openDefaultStrategy();

    void resetFilepath();
    bool askIfWantToDiscard(const Strategy &strategy);

    QFileInfo fileInfo();

    struct Settings {
        inline static const auto lastOpenedDirectoryKey = "lastOpenedDirectory";
        inline static const QString lastOpenedStrategyKey = "lastOpenedStrategy";
        inline static const QString recentFilesKey = "recentFiles";
        inline static const QString defaultStrategyKey = "defaultStrategy";
        inline static const QString defaultActivitiesKey = "defaultActivities";
        inline static const int numberOfRecent = 5;
    };

private:
    /// True if current state is saved in a file
    bool _isSaved = true;

    /// Holds opened file's path
    QString filepath;

    QWidget *window;

    inline static const QString searchPattern =
            QObject::tr("Strategy files (*.stg)");

    static QString destinationDir();
    void write(const Strategy &strategy);
    void updateLastOpened();
    int showAreYouSureDialog();
    void showCantOpenDialog(const QFile &file);
    void showCantSaveDialog(const QSaveFile &file);

};

#endif // FILESYSTEMIOMANAGER_H
