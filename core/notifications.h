//
// Created by Dmitry Khrykin on 26.08.2020.
//

#ifndef STRATEGR_NOTIFICATIONS_H
#define STRATEGR_NOTIFICATIONS_H

#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "file_bookmark.h"
#include "stgstring.h"
#include "time_utils.h"

namespace stg::user_notifications {

    constexpr time_t immediate_delivery_time = 0;

#pragma mark - Notification

    struct notification {
        std::string identifier = make_string_uuid();
        std::string title;
        std::string message;
        time_t delivery_time;

        notification(std::string title,
                     std::string message,
                     time_t delivery_time = immediate_delivery_time,
                     std::shared_ptr<void> user_info = nullptr);

        auto relative_delivery_time() const -> time_utils::seconds;

        template<typename T>
        auto user_info_as() const -> std::shared_ptr<T> {
            return std::static_pointer_cast<T>(user_info);
        }

    private:
        std::shared_ptr<void> user_info;

        friend bool operator==(const notification &lhs, const notification &rhs);
        friend std::ostream &operator<<(std::ostream &os, const notification &notification);
    };

#pragma mark - Notifications Backend

    namespace backend {
        using scheduler_t = std::function<void(const std::vector<notification> &)>;
        using resetter_t = std::function<void(const std::vector<std::string> &)>;
        using sender_t = std::function<void(const notification &)>;
        using path_comparator_t = std::function<bool(const stg::file_bookmark &lhs, const stg::file_bookmark &rhs)>;

#pragma mark - Setting Up Backend

        void set_immediate_sender(sender_t sender);
        void set_scheduler(scheduler_t scheduler);
        void set_resetter(resetter_t resetter);
        void set_path_comparator(path_comparator_t comparator);

#pragma mark - Comparing File Paths Equality

        auto path_equals_to(const stg::file_bookmark &other_path);

#pragma mark - Checking Backend Setup

        auto immediate_notifications_enabled() -> bool;
        auto scheduled_notifications_enabled() -> bool;

#pragma mark - Sending & Scheduling Notifications

        void send_notification(const notification &notification);
        void schedule_notifications(const std::vector<notification> &notifications);
        void delete_notifications(const std::vector<std::string> &identifiers);
    }

#pragma mark - Notifications Storage

    // Keeps record of notification ids associated with the given file.
    class storage {

    public:
        using url_type = stg::file_bookmark;
        using key_type = url_type;
        using value_type = std::vector<std::string>;

    private:
        using data_type = std::unordered_map<key_type, value_type>;

    public:
        using pair_type = data_type::value_type;

        using iterator = data_type::iterator;
        using const_iterator = data_type::const_iterator;

        static auto persisted(const std::string &key) -> storage;

        explicit storage(data_type data = {});

        auto begin() const -> const_iterator;
        auto end() const -> const_iterator;

        auto size() const -> size_t;

        auto find(const url_type &url) -> iterator;
        void insert(const key_type &key, const value_type &value);
        auto erase(const_iterator pos) -> iterator;

        void persist_at(const std::string &key) const;

    private:
        data_type data;

    public:
        friend std::ostream &operator<<(std::ostream &os, const storage &storage);

    private:
        friend auto operator==(const storage &lhs, const storage &rhs) -> bool;
    };
}

#endif//STRATEGR_NOTIFICATIONS_H
