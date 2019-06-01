#include "filesystemiomanager.h"
#include "jsonserializer.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>

FileSystemIOManager::FileSystemIOManager(QWidget *parent) : parent(parent)
{
    destinationDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
}

std::optional<Strategy> FileSystemIOManager::open()
{
    filepath = QFileDialog::getOpenFileName(parent, QObject::tr("Open Strategy"),
                                            destinationDir,
                                            searchPattern);

    qDebug() << "Open filepath" << filepath;
    QFile file(filepath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return std::nullopt;
    };


    QTextStream in(&file);
    auto strategy = JSONSerializer::read(in.readAll());
    file.close();

    return strategy;
}


void FileSystemIOManager::save(const Strategy &strategy)
{
    if (filepath.isEmpty()) {
        saveAs(strategy);
        return;
    }

    write(strategy);
}

void FileSystemIOManager::saveAs(const Strategy &strategy)
{
    filepath = QFileDialog::getSaveFileName(parent, QObject::tr("Save Strategy As"),
                                            destinationDir,
                                            searchPattern);
    write(strategy);
}

bool FileSystemIOManager::isSaved() const
{
    return _isSaved;
}

void FileSystemIOManager::setIsSaved(bool isSaved)
{
    _isSaved = isSaved;
}

QFileInfo FileSystemIOManager::fileInfo()
{
    return QFileInfo(filepath);
}

void FileSystemIOManager::write(const Strategy &strategy)
{
    auto json = JSONSerializer(strategy).write();
    QFile file(filepath);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << json << endl;
    }

    setIsSaved(true);

    file.close();
}
