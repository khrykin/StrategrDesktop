#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H

#include "strategy.h"
#include <QByteArray>
#include <QJsonObject>

class JSONSerializer {
public:
    explicit JSONSerializer(const Strategy &strategy);

    QByteArray write() const;
    static std::optional<Strategy> read(const QString &json);
    static QByteArray writeActivities(const std::vector<Activity> &activities);


private:
    const Strategy &strategy;
    static QJsonObject activityToJson(const Activity &activity);

    static ActivityList readActivities(const QJsonArray &jsonArray);

    static TimeSlotsState readTimeSlots(const QJsonArray &jsonArray,
                                        Strategy::Time beginTime,
                                        Strategy::Duration timeSlotDuration,
                                        const ActivityList &activities);

};

namespace Keys {
    const QString slotDuration = "slotDuration";
    const QString startTime = "startTime";
    const QString activities = "activities";
    const QString numberOfSlots = "numberOfSlots";
    const QString name = "name";
    const QString color = "color";
    const QString timeSlotsKey = "slots";
}; // namespace Keys

#endif // JSONSERIALIZER_H
