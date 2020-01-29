//
// Created by Dmitry Khrykin on 2019-08-13.
//

#ifndef STRATEGR_SELFUPDATER_H
#define STRATEGR_SELFUPDATER_H

#include <functional>
#include <optional>

#include <QFileInfo>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "alert.h"
#include "applicationsettings.h"

class SelfUpdater : public QObject {
public:
    using QObject::QObject;
    using ProgressCallback = std::function<void(float,
                                                const std::optional<QString> &error)>;
    using CheckCallback = std::function<void(bool,
                                             const QString &,
                                             const std::optional<QString> &error)>;

    void check(const CheckCallback &callback) const;
    void performUpdate(const ProgressCallback &progressCallback = nullptr) const;
private:
    static constexpr auto latestReleaseURLTemplate = "https://api.github.com/repos/%1/releases/latest";

    static constexpr auto updateAssetName =
#if defined(Q_OS_MAC)
            "macOS_update.zip"
#elif defined(Q_OS_WIN32) && defined(Q_PROCESSOR_X86_32)
    "win32_update.zip"
#elif defined(Q_OS_WIN32) && defined(Q_PROCESSOR_X86_64)
    "win64_update.zip"
#else
    ""
#endif
    ;

    static constexpr auto skipVersion = "";
    static QString archivePath;

    static std::optional<QUrl> updateDownloadURL;
    static QUrl latestReleaseURL();
    static QStringList parseTag(const QString &tag);

    void handleUpdateReply(QNetworkReply *reply,
                           const ProgressCallback &progressCallback) const;
    void handleDownloadReply(QNetworkAccessManager *networkManager,
                             QNetworkReply *reply) const;

    void handleCheckReply(QNetworkReply *reply,
                          const CheckCallback &callback) const;
    void showWarning(const QNetworkReply *reply) const;

    void setUpdateDownloadURL(const QJsonObject &releaseJsonObject) const;

    bool needsToUpdateTo(const QString &latestTag) const;

    void install() const;

    QString copyDestination() const;
    QString updaterDestination() const;
    QString launchDestination() const;
    QString extractedDirPath() const;
    QString archiveDirPath() const;

    QFileInfo archiveFileInfo() const;

    const char *unzipCommand() const;
    const char *copyCommand() const;
    const char *updaterCommand() const;
    const char *extractedDirName() const;

    void performCopyProcess() const;
    void performUpdaterProcess() const;
    void performUnzipProcess() const;
};


#endif //STRATEGR_SELFUPDATER_H
