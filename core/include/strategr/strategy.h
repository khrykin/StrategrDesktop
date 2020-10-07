#ifndef STRATEGR_STRATEGY_H
#define STRATEGR_STRATEGY_H

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "activity.h"
#include "activitylist.h"
#include "dragoperation.h"
#include "notifiableonchange.h"
#include "resizeoperation.h"
#include "sessionslist.h"
#include "stgstring.h"
#include "strategyhistory.h"
#include "timeslotsstate.h"

namespace stg {
    class strategy : public notifiable_on_change {
    public:
        using duration_t = time_slot::minutes;
        using time_t = time_slot::minutes;
        using seconds = unsigned;
        using time_slot_index_t = time_slots_state::index_t;
        using session_index_t = sessions_list::index_t;
        using size_t = time_slots_state::size_t;

        struct defaults {
            static constexpr auto time_slot_duration = 15;
            static constexpr auto begin_time = 6 * 60;
            static constexpr auto number_of_time_slots = (22 - 6) * 60 / time_slot_duration;
        };

        struct file_write_exception : public std::exception {
        };

        struct file_read_exception : public std::exception {
        };

        struct event {
            std::string name;
            stg::color color;
            time_t begin_time = 0;
            time_t end_time = 0;
        };

        static constexpr auto no_activity = time_slot::no_activity;

#pragma mark - Constructors & Operators

        explicit strategy(time_t begin_time_t = defaults::begin_time,
                          duration_t time_slot_duration_t = defaults::time_slot_duration,
                          size_t number_of_time_slots = defaults::number_of_time_slots);

        strategy(const time_slots_state::data_t &time_slots,
                 const activity_list::data_t &activities);

        strategy(const strategy &other);

        auto operator=(const strategy &other) -> strategy &;

#pragma mark - Import & Export

        static auto from_json_string(const std::string &json_string) -> std::unique_ptr<strategy>;
        auto to_json_string() const -> std::string;

        static auto from_file(const std::string &path) noexcept(false) -> std::unique_ptr<strategy>;
        void write_to_file(const std::string &path) const noexcept(false);

        void save_as_default() const;
        static auto from_default() -> strategy;

        void import_events(const std::vector<event> &events, bool override);

#pragma mark - Collections

        auto activities() const -> const activity_list &;
        auto sessions() const -> const sessions_list &;
        auto time_slots() const -> const time_slots_state &;

#pragma mark - Time Grid Properties

        auto begin_time() const -> time_t;
        void set_begin_time(time_t begin_time);

        auto time_slot_duration() const -> duration_t;
        void set_time_slot_duration(duration_t time_slot_duration);

        auto end_time() const -> time_t;
        void set_end_time(time_t end_time);

        auto number_of_time_slots() const -> size_t;
        auto duration() const -> duration_t;

#pragma mark - Real-Time Properties

        auto active_session() const -> const session *;
        auto upcoming_session() const -> const session *;
        auto progress() const -> float;

#pragma mark - Operations On Activities

        void add_activity(const activity &activity) noexcept(false);
        void silently_add_activity(const activity &activity) noexcept(false);
        void delete_activity(activity_index_t activity_index);
        void silently_delete_activity(activity_index_t activity_index);
        void edit_activity(activity_index_t activity_index, const activity &new_activity) noexcept(false);
        void silently_edit_activity(activity_index_t activity_index, const activity &new_activity) noexcept(false);
        void drag_activity(activity_index_t from_index, activity_index_t to_index);
        void silently_drag_activity(activity_index_t from_index, activity_index_t to_index);
        void reorder_activities_by_usage();

#pragma mark - Operations On Slots

        void place_activity(activity_index_t activity_index, const std::vector<time_slot_index_t> &time_slot_indices);
        void make_empty_at(const std::vector<time_slot_index_t> &time_slot_indices);
        void shift_below_time_slot(time_slot_index_t from_index, int length);

        auto is_resizing() const -> bool;
        void begin_resizing();
        void fill_time_slots_shifting(time_slot_index_t from_index, time_slot_index_t till_index);
        void fill_time_slots(time_slot_index_t from_index, time_slot_index_t till_index);
        void end_resizing();

        auto is_dragging() const -> bool;
        void begin_dragging(session_index_t session_index);
        auto drag_session(session_index_t session_index, int distance) -> sessions_list::index_t;
        void end_dragging();
        void cancel_dragging();

        void copy_session(session_index_t session_index, time_slot_index_t begin_index);

        void copy_slots(time_slot_index_t from_index,
                        time_slot_index_t till_index,
                        time_slot_index_t destination_index);

#pragma mark - History

        void commit_to_history();
        void undo();
        void redo();

        auto has_undo() -> bool;
        auto has_redo() -> bool;

        auto has_activities_undo() -> bool;
        auto has_activities_redo() -> bool;

    private:
        activity_list _activities;
        time_slots_state _time_slots;
        sessions_list _sessions;
        strategy_history history;

        std::unique_ptr<drag_operation> current_drag_operation = nullptr;
        std::unique_ptr<resize_operation> current_resize_operation = nullptr;

        void time_slots_changed();
        void setup_time_slots_callback();

        // current session, may be empty
        auto get_current_session() const -> const session *;

        auto make_history_entry() -> strategy_history::entry;
        void apply_history_entry(const std::optional<strategy_history::entry> &history_entry);

        auto global_slot_indices_from_session(const session &session) const -> std::vector<time_slot_index_t>;
    };
}

#endif// STRATEGR_STRATEGY_H
