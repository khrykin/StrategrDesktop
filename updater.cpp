//
// Created by Dmitry Khrykin on 2019-08-13.
//

#include <QCoreApplication>
#include <QtNetwork>
#include <QtCore>
#include <QObject>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    auto nam = QNetworkAccessManager();
    QObject::connect(&nam, &QNetworkAccessManager::finished,
                     [](QNetworkReply *reply) {
                         if (reply->error() == QNetworkReply::NoError) {

                             QStringList propertyNames;
                             QStringList propertyKeys;

                             QString strReply = (QString) reply->readAll();

                             qDebug() << strReply;

                             QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());

                             QJsonObject jsonObject = jsonResponse.object();

                             qDebug() << "jsonObject" << jsonObject;

                         } else {
                             qDebug() << "ERROR";
                         }
                     });

    QUrl url("https://api.github.com/repos/khrykin/StrategrDesktop/releases/latest");
    QNetworkReply *reply = nam.get(QNetworkRequest(url));

    qDebug() << "Reply" << reply;


    return QCoreApplication::exec();
}