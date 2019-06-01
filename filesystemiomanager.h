#ifndef FILESYSTEMIOMANAGER_H
#define FILESYSTEMIOMANAGER_H

#include "strategy.h"
#include <QObject>
#include <QFileInfo>

class FileSystemIOManager
{
public:
    FileSystemIOManager(QWidget *parent);

    std::optional<Strategy> open();
    void save(const Strategy &strategy);
    void saveAs(const Strategy &strategy);
    bool isSaved() const;
    void setIsSaved(bool isSaved);
    QFileInfo fileInfo();

private:
    void write(const Strategy &strategy);
    bool _isSaved = true;
    QString destinationDir;
    QString filepath;

    QWidget *parent;

    inline static const QString searchPattern = QObject::tr("Strategy files (*.stg)");
};


#endif // FILESYSTEMIOMANAGER_H
