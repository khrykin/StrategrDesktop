//
// Created by Dmitry Khrykin on 2019-08-13.
//

#ifndef STRATEGR_UPDATECHECKER_H
#define STRATEGR_UPDATECHECKER_H

#include <functional>
#include <optional>

#include <QtNetwork>
#include <QApplication>

#include "alert.h"
#include "applicationsettings.h"

class UpdateChecker : public QObject {
public:
    using QObject::QObject;

    void check(const std::function<void(bool)> &callback) const {
        auto *networkManager = new QNetworkAccessManager();
        QObject::connect(networkManager,
                         &QNetworkAccessManager::finished,
                         [=](QNetworkReply *reply) {
                             handleCheckReply(reply, callback);
                             networkManager->deleteLater();
                             reply->deleteLater();
                             networkManager->deleteLater();
                         });

        auto request = QNetworkRequest(latestReleaseURL());
        networkManager->get(request);
    }

    void performUpdate() const {
        auto *networkManager = new QNetworkAccessManager();
        QObject::connect(networkManager,
                         &QNetworkAccessManager::finished,
                         [=](QNetworkReply *reply) {
                             handleUpdateReply(reply);
                             reply->deleteLater();
                             networkManager->deleteLater();
                         });


        if (!updateDownloadURL)
            return;

        auto request = QNetworkRequest(*updateDownloadURL);
        networkManager->get(request);
    }

    void handleUpdateReply(QNetworkReply *reply) const {
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
                                 qDebug() << "downloadProgress" << progress;
                             });

            QObject::connect(networkManager,
                             &QNetworkAccessManager::finished,
                             [=](QNetworkReply *reply) {
                                 handleDownloadReply(networkManager, reply);
                             });

            qDebug() << "Redirect URI" << redirectUrl;

        } else {
            showWarning(reply);
        }
    }

    void handleDownloadReply(QNetworkAccessManager *networkManager,
                             QNetworkReply *reply) const {
        if (reply->error() == QNetworkReply::NoError) {
            auto fileName = updateDownloadURL->fileName();
            QFile localFile(QApplication::applicationDirPath() + "/" + fileName);

            if (localFile.open(QIODevice::WriteOnly)) {

                localFile.write(reply->readAll());
                localFile.close();

                qDebug() << "File downloaded";
            }

            reply->deleteLater();

            if (networkManager)
                networkManager->deleteLater();
        } else {
            showWarning(reply);
        }
    }

    void handleCheckReply(QNetworkReply *reply, const std::function<void(bool)> &callback) const {
        if (reply->error() == QNetworkReply::NoError) {
            auto strReply = static_cast<QString>(reply->readAll());
            auto jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
            auto jsonObject = jsonResponse.object();
            auto latestTag = jsonObject["tag_name"].toString();
            if (latestTag.isEmpty())
                return callback(false);

            auto hasUpdates = needsToUpdateTo(latestTag);
            if (hasUpdates) {
                setUpdateDownloadURL(jsonObject);
            }

            return callback(hasUpdates);
        } else {
            showWarning(reply);
        }
    }

    void showWarning(const QNetworkReply *reply) const {
        Alert::showWarning(nullptr,
                           "Couldn't check for updates",
                           reply->errorString());
    }

    void setUpdateDownloadURL(const QJsonObject &releaseJsonObject) const {
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


    bool needsToUpdateTo(const QString &latestTag) const {
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

    static QStringList parseTag(const QString &tag) {
        auto majorMatch = QRegularExpression("^v(\\d+)\\..+").match(tag);
        auto minorMatch = QRegularExpression("^v\\d+\\.(\\d+)\\..+").match(tag);
        auto pathMatch = QRegularExpression("^v\\d+\\.\\d+\\.(\\d+)\\.*").match(tag);

        auto result = QStringList();

        result.append(majorMatch.captured(1));
        result.append(minorMatch.captured(1));
        result.append(pathMatch.captured(1));

        return result;
    }

private:
    static constexpr auto latestReleaseURLTemplate = "https://api.github.com/repos/%1/releases/latest";

    static constexpr auto updateAssetName =
#if defined(Q_OS_MAC)
            "macOS_update.zip"
#elif defined(Q_OS_WIN32) && defined(Q_PROCESSOR_X86_32)
    "win32_update.zip"
#elif defined(Q_OS_WIN32) && defined(Q_PROCESSOR_X86_64)
    "win64_update.zip"
#endif
    ;

    static constexpr auto skipVersion = "";

    static std::optional<QUrl> updateDownloadURL;
    static QUrl latestReleaseURL();

    QVector<QNetworkAccessManager *> networkManagers;
};


#endif //STRATEGR_UPDATECHECKER_H
