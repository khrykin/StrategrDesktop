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
    void setWindow(QWidget *window);
    bool isSaved() const;
    void setIsSaved(bool isSaved);

    QString getOpenFileName();

    void save(const stg::strategy &strategy);
    void saveAs(const stg::strategy &strategy);
    void saveAsDefault(const stg::strategy &strategy);

    std::unique_ptr<stg::strategy> read(const QString &readFilepath);
    static std::optional<QString> lastOpenedFilePath();

    std::unique_ptr<stg::strategy> openDefault();
    std::unique_ptr<stg::strategy> openLastOrDefault();

    void resetFilepath();
    bool askIfWantToDiscardOrLeaveCurrent(const stg::strategy &strategy);

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

    static constexpr auto searchPattern = "stg::strategy files (*.stg)";

    static QString destinationDir();
    void write(const stg::strategy &strategy);
    void updateLastOpened();
    int showAreYouSureDialog();
    void showCantOpenDialog(const QFile &file,
                            const QString &path,
                            const QString &errorMessage);
    void showCantSaveDialog(const QSaveFile &file);

};

#endif // FILESYSTEMIOMANAGER_H
