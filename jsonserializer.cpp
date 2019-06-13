#include "jsonserializer.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>

JSONSerializer::JSONSerializer(const Strategy &strategy) : strategy(strategy) {}

QByteArray JSONSerializer::write() const {
  QJsonObject json;
  json[Keys::slotDuration] = strategy.slotDuration();
  json[Keys::startTime] = strategy.startTime();
  json[Keys::numberOfSlots] = static_cast<int>(strategy.numberOfSlots());

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

  if (obj.contains(Keys::numberOfSlots) &&
      obj[Keys::numberOfSlots].isDouble()) {
    strategy.setNumberOfSlots(
        static_cast<unsigned int>(obj[Keys::numberOfSlots].toDouble()));
  }

  if (obj.contains(Keys::activities) && obj[Keys::activities].isArray()) {
    std::vector<Activity> activities;

    for (auto activityJsonRef : obj[Keys::activities].toArray()) {
      auto activityJson = activityJsonRef.toObject();
      if (activityJson.contains(Keys::name) &&
          activityJson[Keys::name].isString()) {
        activities.push_back(
            Activity(activityJson[Keys::name].toString().toStdString(),
                     activityJson[Keys::color].toString().toStdString()));
      }
    };

    strategy.activities = activities;
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

  qDebug().noquote() << QString::fromStdString(strategy.debugSlots());

  return std::make_optional<Strategy>(strategy);
}

QJsonObject JSONSerializer::activityToJson(const Activity &activity) {
  QJsonObject activityJson;
  activityJson["name"] = QString::fromStdString(activity.name);
  activityJson["color"] = QString::fromStdString(activity.color);
  return activityJson;
}
