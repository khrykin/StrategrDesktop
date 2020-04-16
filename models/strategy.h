#ifndef STRATEGR_STRATEGY_H
#define STRATEGR_STRATEGY_H

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "notifiableonchange.h"
#include "activity.h"
#include "activitylist.h"
#include "timeslotsstate.h"
#include "sessionslist.h"
#include "strategyhistory.h"
#include "dragoperation.h"
#include "resizeoperation.h"

namespace stg {
    class strategy : public notifiable_on_change {
    public:
        struct defaults {
            static constexpr auto time_slot_duration = 15;
            static constexpr auto begin_time = 6 * 60;
            static constexpr auto number_of_time_slots = (22 - 6) * 60 / time_slot_duration;
        };

        struct file_write_exception : public std::exception {
        };
        struct file_read_exception : public std::exception {
        };

        using duration_t = time_slot::duration_t;
        using time_t = time_slot::time_t;
        using time_slot_index_t = time_slots_state::index_t;
        using session_index_t = sessions_list::index_t;
        using size_t = time_slots_state::size_t;

        static constexpr auto no_activity = time_slot::no_activity;

#pragma mark - Constructors & Operators

        explicit strategy(time_t begin_time_t = defaults::begin_time,
                          duration_t time_slot_duration_t = defaults::time_slot_duration,
                          size_t number_of_time_slots = defaults::number_of_time_slots);

        strategy(const time_slots_state::data_t &time_slots,
                 const activity_list::data_t &activities);

        strategy(const strategy &other);

        strategy &operator=(const strategy &other);

#pragma mark - Import & Export

        static std::unique_ptr<strategy> from_json_string(const std::string &json_string);
        std::string to_json_string() const;

        static std::unique_ptr<strategy> from_file(const std::string &path) noexcept(false);
        void write_to_file(const std::string &path) const noexcept(false);

#pragma mark - Collections

        const activity_list &activities() const;
        const sessions_list &sessions() const;
        const time_slots_state &time_slots() const;

#pragma mark - Time Grid Properties

        time_t begin_time() const;
        void set_begin_time(time_t begin_time);

        duration_t time_slot_duration() const;
        void set_time_slot_duration(duration_t time_slot_duration);

        size_t number_of_time_slots() const;
        void set_number_of_time_slots(size_t number_of_time_slots);

        time_t end_time() const;
        duration_t duration() const;

#pragma mark - Real-time Properties

        const session *active_session() const;
        const session *upcoming_session() const;

#pragma mark - Operations On Activities

        void add_activity(const activity &activity) noexcept(false);
        void silently_add_activity(const activity &activity) noexcept(false);
        void delete_activity(activity_index activity_index);
        void silently_delete_activity(activity_index activity_index);
        void edit_activity(activity_index activity_index, const activity &new_activity) noexcept(false);
        void silently_edit_activity(activity_index activity_index, const activity &new_activity) noexcept(false);
        void drag_activity(activity_index from_index, activity_index to_index);
        void silently_drag_activity(activity_index from_index, activity_index to_index);
        void reorder_activities_by_usage();

#pragma mark - Operations On Slots

        void place_activity(activity_index activity_index, const std::vector<time_slot_index_t> &time_slot_indices);
        void make_empty_at(const std::vector<time_slot_index_t> &time_slot_indices);
        void shift_below_time_slot(time_slot_index_t from_index, int length);

        bool is_resizing();
        void begin_resizing();
        void fill_time_slots_shifting(time_slot_index_t from_index, time_slot_index_t till_index);
        void fill_time_slots(time_slot_index_t from_index, time_slot_index_t till_index);
        void end_resizing();

        bool is_dragging();
        void begin_dragging(session_index_t session_index);
        stg::sessions_list::index_t drag_session(session_index_t session_index, int distance);
        void end_dragging();
        void cancel_dragging();

        void copy_session(session_index_t session_index, time_slot_index_t begin_index);

#pragma mark - History

        void commit_to_history();
        void undo();
        void redo();
        bool has_activities_undo();
        bool has_activities_redo();
    private:
        activity_list _activities;
        time_slots_state _time_slots;
        sessions_list _sessions;
        strategy_history history;

        std::shared_ptr<drag_operation> current_drag_operation = nullptr;
        std::shared_ptr<resize_operation> current_resize_operation = nullptr;

        void time_slots_changed();
        void setup_time_slots_callback();

        // current session, may be empty
        const session *get_current_session() const;

        strategy_history::entry make_history_entry();
        void apply_history_entry(const std::optional<strategy_history::entry> &history_entry);

        std::vector<time_slot_index_t>
        global_slot_indices_from_session(const session &session) const;
    };
}

#endif // STRATEGR_STRATEGY_H
