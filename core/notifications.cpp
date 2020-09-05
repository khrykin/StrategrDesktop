//
// Created by Dmitry Khrykin on 26.08.2020.
//

#include "notifications.h"

#include <utility>
#include "time_utils.h"
#include "persistent.h"

namespace stg::user_notifications {

#pragma mark - Notification

    notification::notification(std::string title,
                               std::string message,
                               time_t delivery_time,
                               std::shared_ptr<void> user_info)
            : title(std::move(title)),
              message(std::move(message)),
              delivery_time(delivery_time),
              user_info(std::move(user_info)) {}

    auto notification::relative_delivery_time() const -> time_utils::seconds {
        return time_utils::seconds_from_calendar_time(delivery_time);
    }

    auto operator==(const notification &lhs, const notification &rhs) -> bool {
        // Two notifications are considered equal if all properties other than id are equal,
        return lhs.title == rhs.title &&
               lhs.message == rhs.message &&
               lhs.delivery_time == rhs.delivery_time;
    }

    auto operator<<(std::ostream &os, const notification &notification) -> std::ostream & {
        os << "notification: [ ";
        os << "id: \"" << notification.identifier << "\", ";
        os << "title: \"" << notification.title << "\", ";
        os << "message: \"" << notification.message << "\", ";
        os << "delivery_time: \"" << time_utils::to_string(notification.delivery_time) << "\"";
        os << "]";

        return os;
    }

#pragma mark - Notifications Backend

    namespace backend {
        // Asks delegate to send notification immediately.
        // Called when using polling method.
        static sender_t on_send_notification;

        // Asks delegate to schedule notifications.
        // Called when using scheduling method.
        static scheduler_t on_schedule_notifications;

        // Asks delegate to delete previously scheduled notifications with identifiers.
        // Called when using scheduling method.
        static resetter_t on_delete_notifications;

        // Asks delegate to check if two paths are pointing to the same file.
        // Called when using scheduling method.
        static path_comparator_t paths_comparator;


#pragma mark - Setting Up Backend

        void set_immediate_sender(sender_t sender) {
            on_send_notification = std::move(sender);
        }

        void set_scheduler(scheduler_t scheduler) {
            on_schedule_notifications = std::move(scheduler);
        }

        void set_resetter(resetter_t resetter) {
            on_delete_notifications = std::move(resetter);
        }

        void set_path_comparator(path_comparator_t comparator) {
            paths_comparator = std::move(comparator);
        }

#pragma mark - Comparing File Paths Equality

        auto path_equals_to(const stg::file_bookmark &other_path) {
            return [&](const storage::pair_type &dictionary_pair) {
                if (!paths_comparator)
                    return dictionary_pair.first == other_path;

                return paths_comparator(dictionary_pair.first, other_path);
            };
        }
    }

#pragma mark - Checking Backend Setup

    auto backend::immediate_notifications_enabled() -> bool {
        return !!on_send_notification;
    }

    auto backend::scheduled_notifications_enabled() -> bool {
        return !!on_schedule_notifications && !!on_delete_notifications;
    }

#pragma mark - Sending & Scheduling Notifications

    void backend::send_notification(const notification &notification) {
        assert(on_send_notification);
        on_send_notification(notification);
    }

    void backend::schedule_notifications(const std::vector<notification> &notifications) {
        assert(on_schedule_notifications);
        on_schedule_notifications(notifications);
    }

    void backend::delete_notifications(const std::vector<std::string> &identifiers) {
        assert(on_delete_notifications);
        on_delete_notifications(identifiers);
    }

#pragma mark - Notifications Storage

    auto storage::persisted(const std::string &key) -> storage {
        auto dictionary_ptr = persistent_storage::get<storage>(key);

        return dictionary_ptr ? *dictionary_ptr : storage();
    }

    storage::storage(storage::data_type data) : data(std::move(data)) {}

    auto storage::begin() const -> const_iterator {
        return data.begin();
    }

    auto storage::end() const -> const_iterator {
        return data.end();
    }

    auto storage::size() const -> size_t {
        return data.size();
    }

    auto storage::find(const url_type &url) -> iterator {
        auto it = backend::paths_comparator != nullptr
                  ? std::find_if(data.begin(), data.end(), backend::path_equals_to(url))
                  : data.find(url);

        return it;
    }


    void storage::insert(const key_type &key, const value_type &value) {
        auto file_it = find(key);
        if (file_it != end())
            file_it->second = value;
        else
            data[key] = value;
    }

    auto storage::erase(const_iterator pos) -> iterator {
        return data.erase(pos);
    }

    void storage::persist_at(const std::string &key) const {
        persistent_storage::set(key, *this);

        std::cout << "Persisting notifications for " << data.size() << " files:\n";
        for (auto &[key, value] : data) {
            std::cout << "\t\"" << key.to_string() << "\" (" << value.size() << ")\n";
        }
    }

    auto operator==(const storage &lhs, const storage &rhs) -> bool {
        return lhs.data == rhs.data;
    }

    std::ostream &operator<<(std::ostream &os, const storage &storage) {
        os << "user_notifications::storage(\n";

        for (auto &[bookmark, ids]: storage) {
            os << "\t" << bookmark << ":\n";
            for (auto &id : ids) {
                os << "\t\t\"" << id << "\"\n";
            }
        }

        os << ")";

        return os;
    }
}
