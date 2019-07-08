#ifndef FILESYSTEMIOMANAGER_H
#define FILESYSTEMIOMANAGER_H

#include "models/strategy.h"
#include <QFileInfo>
#include <QObject>

class FileSystemIOManager {
public:
  FileSystemIOManager(QWidget *parent);

  std::optional<Strategy> open();
  void save(const Strategy &strategy);
  void saveAs(const Strategy &strategy);
  void saveAsDefault(const Strategy &strategy);
  void saveActivitiesAsDefault(const std::vector<Activity> &activities);

  std::optional<Strategy> read(QString readFilepath);
  std::optional<Strategy> lastOpened();
  void resetFilepath();
  void clearRecent();

  bool isSaved() const;
  void setIsSaved(bool isSaved);
  std::optional<Strategy> defaultStrategy();

  bool defaultStrategyIsSet();

  QFileInfo fileInfo();
  QStringList recentPaths();
  QStringList recentFileNames();

  struct Settings {
    inline static const QString lastOpenedDirectoryKey = "lastOpenedDirectory";
    inline static const QString lastOpenedStrategyKey = "lastOpenedStrategy";
    inline static const QString recentKey = "recent";
    inline static const QString defaultStrategyKey = "defaultStrategy";
    inline static const QString defaultActivitiesKey = "defaultActivities";
    inline static const int numberOfRecent = 5;
  };

private:
  void write(const Strategy &strategy);
  bool _isSaved = true;
  QString destinationDir();
  QString filepath;

  QWidget *parent;

  void updateLastOpened();

  inline static const QString searchPattern =
      QObject::tr("Strategy files (*.stg)");
};

#endif // FILESYSTEMIOMANAGER_H
