//
// Created by Dmitry Khrykin on 2019-08-13.
//
#include <QStandardPaths>
#include <QProcess>

#include "selfupdater.h"

QString SelfUpdater::archivePath;

QUrl SelfUpdater::latestReleaseURL() {
    return QUrl(QString(latestReleaseURLTemplate)
                        .arg(ApplicationSettings::repoName));
}

std::optional<QUrl> SelfUpdater::updateDownloadURL = std::nullopt;

void SelfUpdater::check(const CheckCallback &callback) const {
    auto *networkManager = new QNetworkAccessManager();
    QObject::connect(networkManager,
                     &QNetworkAccessManager::finished,
                     [=](QNetworkReply *reply) {
                         handleCheckReply(reply, callback);
                         networkManager->deleteLater();
                         reply->deleteLater();
                     });

    auto request = QNetworkRequest(latestReleaseURL());
    networkManager->get(request);
}

void SelfUpdater::performUpdate(const ProgressCallback &progressCallback) const {
    auto *networkManager = new QNetworkAccessManager();
    QObject::connect(networkManager,
                     &QNetworkAccessManager::finished,
                     [=](QNetworkReply *reply) {
                         handleUpdateReply(reply, progressCallback);
                         reply->deleteLater();
                         networkManager->deleteLater();
                     });


    if (!updateDownloadURL)
        return;

    auto request = QNetworkRequest(*updateDownloadURL);
    networkManager->get(request);
}

void SelfUpdater::handleUpdateReply(QNetworkReply *reply,
                                    const ProgressCallback &progressCallback) const {
    if (reply->error() == QNetworkReply::NoError) {
        auto redirectUrl =
                reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

        if (redirectUrl.isEmpty()) {
            return handleDownloadReply(nullptr, reply);
        }

        auto *networkManager = new QNetworkAccessManager();
        auto request = QNetworkRequest(redirectUrl);
        auto *downloadReply = networkManager->get(request);

        QObject::connect(downloadReply,
                         &QNetworkReply::downloadProgress,
                         [=](qint64 bytesReceived, qint64 bytesTotal) {
                             auto progress = static_cast<double>(bytesReceived) / static_cast<double>(bytesTotal);
                             progressCallback(progress, std::nullopt);
                         });

        QObject::connect(networkManager,
                         &QNetworkAccessManager::finished,
                         [=](QNetworkReply *reply) {
                             handleDownloadReply(networkManager, reply);
                         });
    } else {
        showWarning(reply);
        progressCallback(0, reply->errorString());
    }
}

void SelfUpdater::handleDownloadReply(QNetworkAccessManager *networkManager,
                                      QNetworkReply *reply) const {
    if (reply->error() == QNetworkReply::NoError) {
        auto fileName = updateDownloadURL->fileName();
        auto updateArchiveFolder = QDir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
        archivePath = updateArchiveFolder.absoluteFilePath(fileName);
        QFile localFile(archivePath);

        if (localFile.open(QIODevice::WriteOnly)) {
            localFile.write(reply->readAll());
            localFile.close();

            install();
        }

        reply->deleteLater();

        if (networkManager)
            networkManager->deleteLater();
    } else {
        showWarning(reply);
    }
}

void SelfUpdater::handleCheckReply(QNetworkReply *reply,
                                   const CheckCallback &callback) const {
    if (reply->error() == QNetworkReply::NoError) {
        auto strReply = static_cast<QString>(reply->readAll());
        auto jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        auto jsonObject = jsonResponse.object();
        auto latestTag = jsonObject["tag_name"].toString();
        if (latestTag.isEmpty())
            return callback(false, "", std::nullopt);

        auto hasUpdates = needsToUpdateTo(latestTag);
        QString updateVersion;

        if (hasUpdates) {
            updateVersion = parseTag(latestTag).join(".");
            setUpdateDownloadURL(jsonObject);
        }

        return callback(hasUpdates, updateVersion, std::nullopt);
    } else {
        showWarning(reply);
        callback(false, "", reply->errorString());
    }
}

void SelfUpdater::showWarning(const QNetworkReply *reply) const {
    Alert::showWarning(nullptr,
                       "Couldn't check for updates",
                       reply->errorString());
}

void SelfUpdater::setUpdateDownloadURL(const QJsonObject &releaseJsonObject) const {
    auto assets = releaseJsonObject["assets"].toArray();
    updateDownloadURL = std::nullopt;

    for (const auto &assetRef: assets) {
        auto asset = assetRef.toObject();
        auto assetName = asset["name"].toString();

        if (assetName == updateAssetName) {
            updateDownloadURL = QUrl(asset["browser_download_url"].toString());
            break;
        }
    }
}

bool SelfUpdater::needsToUpdateTo(const QString &latestTag) const {
    auto currentTag = QString("v%1").arg(ApplicationSettings::version);
    if (currentTag == skipVersion) {
        return false;
    }

    auto currentTagVersions = parseTag(currentTag);
    auto latestTagVersions = parseTag(latestTag);

    bool needsToBeUpdated = false;
    if (latestTagVersions[0] > currentTagVersions[0]) {
        needsToBeUpdated = true;
    } else if (latestTagVersions[1] > currentTagVersions[1]) {
        needsToBeUpdated = true;
    } else if (latestTagVersions[2] > currentTagVersions[2]) {
        needsToBeUpdated = true;
    }

    return needsToBeUpdated;
}

QStringList SelfUpdater::parseTag(const QString &tag) {
    auto majorMatch = QRegularExpression("^v(\\d+)\\..+").match(tag);
    auto minorMatch = QRegularExpression("^v\\d+\\.(\\d+)\\..+").match(tag);
    auto pathMatch = QRegularExpression("^v\\d+\\.\\d+\\.(\\d+)\\.*").match(tag);

    auto result = QStringList();

    result.append(majorMatch.captured(1));
    result.append(minorMatch.captured(1));
    result.append(pathMatch.captured(1));

    return result;
}

void SelfUpdater::install() const {
    performUnzipProcess();
    performCopyProcess();
    performUpdaterProcess();
}

void SelfUpdater::performUpdaterProcess() const {
    const auto updaterArguments = QStringList{
            QString::number(QCoreApplication::applicationPid()),
            launchDestination()};

    QProcess updaterProcess;
    updaterProcess.setWorkingDirectory(updaterDestination());
    connect(&updaterProcess,
            &QProcess::errorOccurred,
            this,
            [](QProcess::ProcessError error) {
                qDebug() << "updaterProcess" << error;
            });

    updaterProcess.setProgram(updaterCommand());
    updaterProcess.setArguments(updaterArguments);
    updaterProcess.startDetached();
    updaterProcess.waitForStarted();
}

const char *SelfUpdater::updaterCommand() const {
    const auto updater = "updater";
    return updater;
}

void SelfUpdater::performCopyProcess() const {
    const auto copyArguments = QStringList{"-Rf",
                                           extractedDirName(),
                                           copyDestination()};
    QProcess copyProcess;
    copyProcess.setWorkingDirectory(archiveDirPath());
    connect(&copyProcess,
            &QProcess::errorOccurred,
            this,
            [](QProcess::ProcessError error) {
                qDebug() << "copyProcess" << error;
            });

    copyProcess.start(copyCommand(), copyArguments);
    copyProcess.waitForFinished();

    QDir(extractedDirPath()).removeRecursively();
}

QString SelfUpdater::extractedDirPath() const {
    const auto extractedDirPath = QDir(archiveDirPath()).absoluteFilePath(extractedDirName());
    return extractedDirPath;
}

const char *SelfUpdater::extractedDirName() const {
    const auto extractedDirName = "Strategr.app";
    return extractedDirName;
}

void SelfUpdater::performUnzipProcess() const {
    const auto unzipArguments = QStringList{archiveFileInfo().fileName()};

    qDebug() << "unzipArguments" << unzipArguments;

    QProcess unzipProcess;
    unzipProcess.setWorkingDirectory(archiveDirPath());
    connect(&unzipProcess,
            &QProcess::errorOccurred,
            this,
            [](QProcess::ProcessError error) {
                qDebug() << "unzipProcess" << error;
            });

    unzipProcess.start(unzipCommand(), unzipArguments);
    unzipProcess.waitForFinished();

    QFile::remove(archivePath);
}

QString SelfUpdater::archiveDirPath() const {
    const auto archiveDirPath = archiveFileInfo().absoluteDir().absolutePath();
    return archiveDirPath;
}

QFileInfo SelfUpdater::archiveFileInfo() const {
    const auto archiveFileInfo = QFileInfo(archivePath);
    return archiveFileInfo;
}

const char *SelfUpdater::copyCommand() const {
    const auto copy = "cp";
    return copy;
}

const char *SelfUpdater::unzipCommand() const {
    const auto unzip = "unzip";
    return unzip;
}


QString SelfUpdater::launchDestination() const {
    auto launchDestinationDir = QDir(updaterDestination());
    QString launchDestinantion;

#if defined(Q_OS_MAC)
    launchDestinationDir.cdUp();
    launchDestinationDir.cdUp();
    launchDestinantion = launchDestinationDir.absolutePath();
#elif defined(Q_OS_WIN32)
    launchDestinantion = launchDestinationDir.absolutePath() + "\" + "Strategr.exe";
#else
        launchDestinantion = launchDestinationDir.absolutePath() + "/" + "Strategr";
#endif

    return launchDestinantion;
}

QString SelfUpdater::updaterDestination() const {
    auto updaterPath = QDir(copyDestination()).absoluteFilePath("Strategr");

#ifdef Q_OS_MAC
    updaterPath = updaterPath + ".app/Contents/MacOS";
#endif

    return updaterPath;
}

QString SelfUpdater::copyDestination() const {
    auto copyDistination = QDir(QCoreApplication::applicationDirPath());
    copyDistination.cdUp();

#ifdef Q_OS_MAC
    copyDistination.cdUp();
    copyDistination.cdUp();
#endif

    return copyDistination.absolutePath();
}
