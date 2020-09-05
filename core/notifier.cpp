//
// Created by Dmitry Khrykin on 2019-11-26.
//

#include <algorithm>
#include <cassert>
#include <utility>
#include <cstdlib>

#include "notifier.h"
#include "strategy.h"
#include "time_utils.h"
#include "timer.h"
#include "notifications.h"
#include "persistent.h"

namespace stg {
    constexpr auto *notifications_dictionary_key = "activeURLs";

#pragma mark - Notification

    static auto make_notification_title(const session &session, notification_type type) -> std::string {
        if (type == notification_type::prepare_strategy_end ||
            type == notification_type::strategy_end) {
            return "End Of A Strategy";
        }

        if (!session.activity)
            throw std::invalid_argument("session must have an activity for this type of notification");

        return session.activity->name()
               + " ("
               + time_utils::human_string_from_minutes(session.duration())
               + ")";
    }

    static auto make_notification_message(const session &session, notification_type type) -> std::string {
        const auto minutes_interval = notifier::prepare_seconds_interval / 60;
        const auto time_string = time_utils::human_string_from_minutes(minutes_interval);

        switch (type) {
            case notification_type::prepare_start:
                return "Coming up in " + time_string;
            case notification_type::start:
                return "Starts right now";
            case notification_type::prepare_end:
                return "Ends in " + time_string;
            case notification_type::end:
                return "Ends right now";
            case notification_type::prepare_strategy_end:
                return "Strategy ends in " + time_string;
            case notification_type::strategy_end:
                return "Strategy ends right now";
            default:
                return "";
        }
    }

    static auto make_notification_delivery_time(const session &session, notification_type type) -> time_t {
        auto make_relative_time = [=]() -> notifier::seconds {
            switch (type) {
                case notification_type::prepare_start:
                    return notifier::prepare_delivery_seconds(session.begin_time());
                case notification_type::start:
                    return notifier::immediate_delivery_seconds(session.begin_time());
                case notification_type::prepare_end:
                case notification_type::prepare_strategy_end:
                    return notifier::prepare_delivery_seconds(session.end_time());
                case notification_type::end:
                case notification_type::strategy_end:
                    return notifier::immediate_delivery_seconds(session.end_time());
                default:
                    return 0;
            }
        };

        auto relative_time = make_relative_time();
        auto day = 24 * 60 * 60;

        if (relative_time >= day) {
            relative_time = relative_time - day;
        }

        return time_utils::calendar_time_from_seconds(relative_time);
    }

    auto session_notification(const session &session,
                              notification_type type) -> user_notifications::notification {


        return user_notifications::notification(make_notification_title(session, type),
                                                make_notification_message(session, type),
                                                make_notification_delivery_time(session, type),
                                                std::make_shared<notification_type>(type));
    }

#pragma mark - Notifier

#pragma mark - Getting Delivery Intervals from Current Time

    auto notifier::immediate_delivery_seconds(minutes minutes_time) -> seconds {
        return minutes_time * 60 - immediate_seconds_interval;
    }

    auto notifier::prepare_delivery_seconds(minutes minutes_time) -> seconds {
        return minutes_time * 60 - prepare_seconds_interval;
    }

#pragma mark - Construction

    notifier::notifier(const stg::strategy &strategy, std::optional<file_bookmark> file)
            : strategy(strategy),
              _file(std::move(file)) {
        // The strategy might have changed before previous notifications were scheduled
        // (e.g. by manipulating the file directly),
        // so we mandatorily reschedule notifications for safety.
        schedule();

        // This effectively updates the list of active files,
        // although persisted notifications ids might be stale until the destruction of this object,
        // and are not intended to be read from persistent storage directly.
        if (_file)
            persist_scheduled_identifiers();

        strategy.sessions().add_on_change_callback(this, &notifier::on_sessions_change);
    }

    notifier::~notifier() {
        if (polling_timer)
            stop_polling();

        if (_file)
            persist_scheduled_identifiers();
    }

#pragma mark - Responding To Strategy Changes

    void notifier::on_sessions_change() {
        auto strategy_is_busy = strategy.is_dragging() || strategy.is_resizing();

        if (on_change_timer)
            on_change_timer->invalidate();

        if (strategy_is_busy) {
            on_change_timer = timer::schedule(1, false, [this] { on_sessions_change(); });
        } else {
            schedule();
        }
    }

#pragma mark - Polling For Notifications

    void notifier::start_polling(timer::seconds interval) {
        assert("Already polling" && polling_timer == nullptr);

        polling_timer = stg::timer::schedule(interval, true, [=] {
            send_now_if_needed(interval);
        });
    }

    void notifier::send_now_if_needed(seconds polling_seconds_interval) {
        using namespace user_notifications;

        auto current_time = time_utils::current_seconds();

        if (last_poll_time && std::abs((int) current_time - (int) last_poll_time) > 4 * polling_seconds_interval) {
            // If time difference between two calls of this function was too big,
            // this probably means that the system time had changed, we need to reschedule.
            schedule();
        }

        last_poll_time = current_time;

        if (_upcoming_notifications.empty() ||
            current_time < _upcoming_notifications.front().relative_delivery_time())
            return;

        // We have to send only last notification for which current time >= delivery time.
        // The first is guaranteed to be so, we need to check the others

        // This points to the first notification after the one we need:
        auto next_notification_it = std::find_if(_upcoming_notifications.begin() + 1,
                                                 _upcoming_notifications.end(),
                                                 [current_time](const notification &notification) {
                                                     return current_time < notification.relative_delivery_time();
                                                 });

        auto current_notification_it = std::prev(next_notification_it);
        auto &notification = *current_notification_it;

        if (user_notifications::backend::immediate_notifications_enabled())
            user_notifications::backend::send_notification(notification);

        // Remove just sent and stale notifications
        _upcoming_notifications.erase(_upcoming_notifications.begin(), next_notification_it);
    }

    void notifier::stop_polling() {
        polling_timer = nullptr;
    }

#pragma mark - Scheduling Notifications

    void notifier::schedule() {
        using namespace user_notifications;

        // Ask delegate to remove previous notifications
        auto previously_scheduled_identifiers = scheduled_identifiers();
        if (backend::scheduled_notifications_enabled() && !previously_scheduled_identifiers.empty())
            backend::delete_notifications(previously_scheduled_identifiers);

        notifications_list notifications;

        for (const auto &session : strategy.sessions()) {
            auto session_index = &session - &strategy.sessions()[0];
            auto next_session_it = strategy.sessions().begin() + session_index + 1;

            if (session.activity) {
                notifications.push_back(session_notification(session, notification_type::prepare_start));
                notifications.emplace_back(session_notification(session, notification_type::start));

                if (next_session_it != strategy.sessions().end() && !next_session_it->activity) {
                    notifications.emplace_back(session_notification(session, notification_type::prepare_end));
                    notifications.emplace_back(session_notification(session, notification_type::end));
                }
            }

            if (next_session_it == strategy.sessions().end()) {
                notifications.emplace_back(session_notification(session, notification_type::prepare_strategy_end));
                notifications.emplace_back(session_notification(session, notification_type::strategy_end));
            }
        }

        _scheduled_notifications = notifications;

        if (user_notifications::backend::scheduled_notifications_enabled())
            backend::schedule_notifications(notifications);

        remove_stale_from(notifications);

        _upcoming_notifications = notifications;
    }

    void notifier::remove_stale_from(notifications_list &notifications) {
        using namespace user_notifications;

        auto current_seconds = stg::time_utils::current_seconds();
        notifications.erase(std::remove_if(notifications.begin(),
                                           notifications.end(),
                                           [current_seconds](const notification &notification) {
                                               return notification.relative_delivery_time() < current_seconds;
                                           }), notifications.end());
    }

    auto notifier::scheduled_identifiers() const -> std::vector<std::string> {
        if (_file && _scheduled_notifications.empty())
            return persisted_notifications_identifiers();

        std::vector<std::string> result;
        std::transform(_scheduled_notifications.begin(),
                       _scheduled_notifications.end(),
                       std::back_inserter(result),
                       [](const auto &notification) {
                           return notification.identifier;
                       });

        return result;
    }

#pragma mark - Manually Persisting Scheduled Notifications Identifiers

    void notifier::persist_scheduled_identifiers() {
        assert("Attempted to save notification identifiers for empty file path" && _file);

        auto dict = user_notifications::storage::persisted(notifications_dictionary_key);
        dict.insert(*_file, scheduled_identifiers());
        dict.persist_at(notifications_dictionary_key);
    }

#pragma mark - Getting & Updating Represented File Path

    auto notifier::file_path() const -> const std::optional<file_bookmark> & {
        return _file;
    }

    void notifier::set_file(const std::optional<file_bookmark> &file_path) {
        using namespace user_notifications;
        using namespace user_notifications;

        auto old_file_path = _file;
        _file = file_path;

        if (!old_file_path && !file_path)
            return;

        auto dict = storage::persisted(notifications_dictionary_key);

        auto old_path_it = dict.find(*old_file_path);
        if (old_path_it != dict.end()) {
            auto scheduled_notification_ids = old_path_it->second;
            if (!file_path && backend::scheduled_notifications_enabled()) {
                // This means that we don't no longer want notifications for this file.
                backend::delete_notifications(scheduled_notification_ids);
            } else {
                // This means that represented file was moved/renamed.
                dict.insert(*file_path, scheduled_notification_ids);
            }

            // In either case, remove old entry from dictionary.
            dict.erase(old_path_it);
            dict.persist_at(notifications_dictionary_key);
        }
    }

    void notifier::note_file_removed(const file_bookmark &file_path) {
        std::cout << "note_file_removed: \"" << file_path << "\"\n";

        using namespace user_notifications;

        auto dict = storage::persisted(notifications_dictionary_key);

        auto file_path_it = dict.find(file_path);
        if (file_path_it != dict.end()) {
            const auto &scheduled_notification_ids = file_path_it->second;

            if (backend::scheduled_notifications_enabled())
                backend::delete_notifications(scheduled_notification_ids);

            dict.erase(file_path_it);
            dict.persist_at(notifications_dictionary_key);
        }
    }

    void notifier::note_file_moved(const file_bookmark &from, const file_bookmark &to) {
        std::cout << "note_file_moved from: \"" << from << "\" to: \"" << to << "\"\n";

        auto dict = user_notifications::storage::persisted(notifications_dictionary_key);
        auto old_path_it = dict.find(from);
        if (old_path_it != dict.end()) {
            dict.insert(to, old_path_it->second);
            dict.erase(old_path_it);

            dict.persist_at(notifications_dictionary_key);
        }
    }

#pragma mark - Getting Active Files

    auto notifier::active_files() -> std::unordered_set<file_bookmark> {
        auto dict = user_notifications::storage::persisted(notifications_dictionary_key);

        std::unordered_set<file_bookmark> files{dict.size()};
        for (auto &[key, _] : dict)
            files.insert(key);

        return files;
    }

    void notifier::reset_active_files() {
        user_notifications::storage().persist_at(notifications_dictionary_key);
    }

#pragma mark - Reading & Writing to Persistent Dictionary

    auto notifier::persisted_notifications_identifiers() const -> std::vector<std::string> {
        assert("Attempted to read notification identifiers for empty file path" && _file);

        auto dict = user_notifications::storage::persisted(notifications_dictionary_key);

        auto it = dict.find(file_bookmark(*_file));

        if (it != dict.end())
            return it->second;

        return {};
    }
}
