#include "jsonserializer.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVector>

JSONSerializer::JSONSerializer(const Strategy &strategy) : strategy(strategy) {}

QByteArray JSONSerializer::write() const {
  QJsonObject json;
  json[Keys::slotDuration] = strategy.slotDuration();
  json[Keys::startTime] = strategy.startTime();

  QJsonArray activities;
  for (auto &activity : strategy.activities) {
    activities.append(activityToJson(activity));
  }

  json[Keys::activities] = activities;

  QJsonArray slotsState;
  for (auto &slot : strategy.slotsState()) {
    if (slot) {
      auto activityIndex = strategy.indexOfActivity(slot.value());
      slotsState.append(
          activityIndex ? QJsonValue(static_cast<int>(activityIndex.value()))
                        : QJsonValue());
    } else {
      slotsState.append(QJsonValue());
    }
  }

  json["slots"] = slotsState;

  return QJsonDocument(json).toJson();
}

std::optional<Strategy> JSONSerializer::read(const QString &json) {
  auto document = QJsonDocument::fromJson(json.toUtf8());
  if (!document.isObject()) {
    // TODO: Wrong format, show alert
    return std::nullopt;
  }

  auto obj = document.object();
  auto strategy = Strategy();
  if (obj.contains(Keys::slotDuration) && obj[Keys::slotDuration].isDouble()) {
    strategy.setSlotDuration(
        static_cast<int>(obj[Keys::slotDuration].toDouble()));
  }

  if (obj.contains(Keys::startTime) && obj[Keys::startTime].isDouble()) {
    strategy.setStartTime(static_cast<int>(obj[Keys::startTime].toDouble()));
  }

  if (obj.contains(Keys::activities) && obj[Keys::activities].isArray()) {
    auto activities = readActivities(obj[Keys::activities].toArray());
    strategy.activities =
        activities ? activities.value() : std::vector<Activity>();
  }

  if (obj.contains(Keys::slotsKey) && obj[Keys::slotsKey].isArray()) {
    SlotsState slotsState;

    for (auto slotsJsonRef : obj[Keys::slotsKey].toArray()) {
      if (slotsJsonRef.isNull()) {
        slotsState.push_back(std::nullopt);
        continue;
      }

      auto activityIndex = static_cast<unsigned int>(slotsJsonRef.toDouble());
      try {
        auto slot = strategy.activities.at(activityIndex);
        slotsState.push_back(slot);
      } catch (const std::out_of_range &) {
        slotsState.push_back(std::nullopt);
      }
    };

    strategy.setSlotsState(slotsState);
  }

  for (auto &activity : strategy.activities) {
    qDebug() << QString::fromStdString(activity.name);
  }

  return std::make_optional<Strategy>(strategy);
}

QByteArray
JSONSerializer::writeActivities(const std::vector<Activity> &activities) {
  QJsonArray jsonActivities;
  for (auto &activity : activities) {
    jsonActivities.append(activityToJson(activity));
  }

  return QJsonDocument(jsonActivities).toJson();
}

std::optional<std::vector<Activity>>
JSONSerializer::readActivities(const QString &json) {
  auto document = QJsonDocument::fromJson(json.toUtf8());
  if (!document.isArray()) {
    // TODO: Wrong format, show alert
    return std::nullopt;
  }

  auto activities = readActivities(document.array());
  return activities ? activities.value() : std::vector<Activity>();
}

QJsonObject JSONSerializer::activityToJson(const Activity &activity) {
  QJsonObject activityJson;
  activityJson["name"] = QString::fromStdString(activity.name);
  activityJson["color"] = QString::fromStdString(activity.color);
  return activityJson;
}

std::optional<std::vector<Activity>>
JSONSerializer::readActivities(const QJsonArray &jsonArray) {
  std::vector<Activity> activities;
  for (auto activityJsonRef : jsonArray) {
    auto activityJson = activityJsonRef.toObject();
    if (activityJson.contains(Keys::name) &&
        activityJson[Keys::name].isString()) {
      activities.push_back(
          Activity(activityJson[Keys::name].toString().toStdString(),
                   activityJson[Keys::color].toString().toStdString()));
    }
  };

  return std::move(activities);
}
