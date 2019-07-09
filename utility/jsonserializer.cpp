#include "jsonserializer.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVector>

JSONSerializer::JSONSerializer(const Strategy &strategy) : strategy(strategy) {}

QByteArray JSONSerializer::write() const {
    QJsonObject json;
    json[Keys::slotDuration] = static_cast<int>(strategy.timeSlotDuration());
    json[Keys::startTime] = static_cast<int>(strategy.beginTime());

    QJsonArray activities;
    for (auto &activity : strategy.activities()) {
        activities.append(activityToJson(*activity));
    }

    json[Keys::activities] = activities;

    QJsonArray timeSlotsJson;
    for (auto &slot : strategy.timeSlots()) {
        if (slot.activity) {
            auto activityIndex = strategy.activities().indexOf(slot.activity);
            auto activityJson = activityIndex
                                ? QJsonValue(static_cast<int>(*activityIndex))
                                : QJsonValue();

            timeSlotsJson.append(activityJson);
        } else {
            timeSlotsJson.append(QJsonValue());
        }
    }

    json["slots"] = timeSlotsJson;

    return QJsonDocument(json).toJson();
}

std::optional<Strategy> JSONSerializer::read(const QString &json) {
    auto document = QJsonDocument::fromJson(json.toUtf8());
    if (!document.isObject()) {
        // TODO: Wrong format, show alert
        return std::nullopt;
    }

    auto obj = document.object();

    Strategy::Duration timeSlotDuration = 0;
    Strategy::Time beginTime = 0;
    ActivityList activities;

    if (obj.contains(Keys::slotDuration) &&
        obj[Keys::slotDuration].isDouble()) {
        timeSlotDuration =
                static_cast<Strategy::Duration>(obj[Keys::slotDuration].toDouble());
    } else {
        timeSlotDuration = Strategy::Defaults::timeSlotDuration;
    }

    if (obj.contains(Keys::startTime) &&
        obj[Keys::startTime].isDouble()) {
        beginTime = static_cast<int>(obj[Keys::startTime].toDouble());
    } else {
        beginTime = Strategy::Defaults::beginTime;
    }

    if (obj.contains(Keys::activities) &&
        obj[Keys::activities].isArray()) {
        activities = readActivities(obj[Keys::activities].toArray());
    }

    if (obj.contains(Keys::timeSlotsKey) &&
        obj[Keys::timeSlotsKey].isArray()) {

        auto slotsJsonArray = obj[Keys::timeSlotsKey].toArray();

        auto timeSlots = readTimeSlots(
                slotsJsonArray,
                beginTime,
                timeSlotDuration,
                activities);

        return Strategy(timeSlots, activities);
    }

    return std::nullopt;
}

QByteArray
JSONSerializer::writeActivities(const std::vector<Activity> &activities) {
    QJsonArray jsonActivities;
    for (auto &activity : activities) {
        jsonActivities.append(activityToJson(activity));
    }

    return QJsonDocument(jsonActivities).toJson();
}

QJsonObject JSONSerializer::activityToJson(const Activity &activity) {
    QJsonObject activityJson;
    activityJson["name"] = QString::fromStdString(activity.name());
    activityJson["color"] = QString::fromStdString(activity.color());
    return activityJson;
}

ActivityList JSONSerializer::readActivities(const QJsonArray &jsonArray) {
    std::vector<Activity> activitiesVector;
    for (const auto &activityJsonRef : jsonArray) {
        auto activityJson = activityJsonRef.toObject();
        if (activityJson.contains(Keys::name) &&
            activityJson[Keys::name].isString()) {

            activitiesVector.emplace_back(
                    Activity(activityJson[Keys::name]
                                     .toString()
                                     .toStdString(),
                             activityJson[Keys::color]
                                     .toString()
                                     .toStdString())
            );
        }
    };

    return ActivityList(activitiesVector);
}

TimeSlotsState
JSONSerializer::readTimeSlots(const QJsonArray &jsonArray,
                              Strategy::Time beginTime,
                              Strategy::Duration timeSlotDuration,
                              const ActivityList &activities) {

    std::vector<TimeSlot> timeSlotsVector;

    for (auto slotIndex = 0; slotIndex < jsonArray.size(); slotIndex++) {
        auto slotsJsonRef = jsonArray[slotIndex];
        auto timeSlotBeginTime = beginTime + slotIndex * timeSlotDuration;

        auto timeSlot = TimeSlot(timeSlotBeginTime,
                                 timeSlotDuration);

        if (!slotsJsonRef.isNull()) {
            auto activityIndex = static_cast<ActivityList::Index>(
                    slotsJsonRef.toDouble()
            );

            try {
                auto activity = activities.at(activityIndex);
                timeSlot.activity = activity;
            } catch (const std::out_of_range &) {
                // Activity is present in time slots, but not present in
                // strategy.activities(), so we won't preserve it.
            }
        }

        timeSlotsVector.push_back(timeSlot);
    };

    return TimeSlotsState(timeSlotsVector);
}
