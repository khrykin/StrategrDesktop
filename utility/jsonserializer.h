#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H

#include "strategy.h"
#include <QByteArray>
#include <QJsonObject>

class JSONSerializer {
public:
    explicit JSONSerializer(const Strategy &strategy);

    QByteArray write() const;
    static std::unique_ptr<Strategy> read(const QString &json);
    static QByteArray writeActivities(const std::vector<Activity> &activities);

private:
    struct Keys {
        static constexpr auto slotDuration = "slotDuration";
        static constexpr auto startTime = "startTime";
        static constexpr auto activities = "activities";
        static constexpr auto name = "name";
        static constexpr auto color = "color";
        static constexpr auto timeSlotsKey = "slots";
    };

    const Strategy &strategy;

    static QJsonObject activityToJson(const Activity &activity);
    static ActivityList readActivities(const QJsonArray &jsonArray);
    static TimeSlotsState readTimeSlots(const QJsonArray &jsonArray,
                                        Strategy::Time beginTime,
                                        Strategy::Duration timeSlotDuration,
                                        const ActivityList &activities);
};


#endif // JSONSERIALIZER_H
