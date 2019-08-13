//
// Created by Dmitry Khrykin on 2019-08-13.
//

#include "updatechecker.h"

QUrl UpdateChecker::latestReleaseURL() {
    return QUrl(QString(latestReleaseURLTemplate)
                        .arg(ApplicationSettings::repoName));
}

std::optional<QUrl> UpdateChecker::updateDownloadURL = std::nullopt;
