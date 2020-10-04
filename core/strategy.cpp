#include <algorithm>
#include <fstream>
#include <map>
#include <vector>

#include "json.h"
#include "persistent.h"
#include "strategy.h"
#include "time_utils.h"

namespace stg {

#pragma mark - Constructors & Operators

    strategy::strategy(time_t begin_time,
                       duration_t time_slot_duration,
                       size_t number_of_time_slots)
        : _time_slots(time_slots_state(begin_time, time_slot_duration, number_of_time_slots)),
          history(make_history_entry()) {

        time_slots_changed();
        setup_time_slots_callback();
    }

    strategy::strategy(const time_slots_state::data_t &time_slots,
                       const activity_list::data_t &activities) : _time_slots(time_slots),
                                                                  _activities(activities),
                                                                  history(make_history_entry()) {

        time_slots_changed();
        setup_time_slots_callback();
    }

    strategy::strategy(const strategy &other) : _time_slots(other._time_slots.data()),
                                                _activities(other._activities.data()),
                                                _sessions(other.sessions().data()),
                                                history(make_history_entry()) {

        setup_time_slots_callback();
    }

    auto stg::strategy::operator=(const strategy &other) -> strategy & {
        _time_slots.reset_with(other.time_slots().data());
        _time_slots.on_change_event();

        _activities.reset_with(other.activities().data());
        _activities.on_change_event();

        history = strategy_history(make_history_entry());

        return *this;
    }

#pragma mark - Import & Export

    auto strategy::from_json_string(const std::string &json_string) -> std::unique_ptr<strategy> {
        return json::parse(json_string);
    }

    auto strategy::to_json_string() const -> std::string {
        return json::serialize(*this);
    }

    auto strategy::from_file(const std::string &path) -> std::unique_ptr<strategy> {
        std::ifstream file(path);

        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            auto json_string = buffer.str();

            file.close();

            return from_json_string(json_string);
        } else {
            throw file_read_exception();
        }
    }

    void strategy::write_to_file(const std::string &path) const {
        auto file = std::ofstream(path);

        if (file.is_open()) {
            file << to_json_string();
            file.close();
        } else {
            throw file_write_exception();
        }
    }

    void strategy::save_as_default() const {
        persistent_storage::set(persistent_storage::keys::default_strategy, to_json_string());
    }

    auto strategy::from_default() -> strategy {
        auto json_string = persistent_storage::get<std::string>(persistent_storage::keys::default_strategy);

        if (json_string) {
            if (auto strategy = stg::strategy::from_json_string(*json_string)) {
                return *strategy;
            }
        }

        return strategy();
    }

    void strategy::import_events(const std::vector<event> &events, bool override = false) {
        std::cout << "imported_events (override: " << override << "): [\n";

        if (override) {
            for (auto &slot : _time_slots._data) {
                slot.activity = no_activity;
            }
        }

        for (const auto &event : events) {
            std::cout << "\tevent: " << event.name
                      << ", color: " << event.color
                      << ", begin_time: " << event.begin_time
                      << ", end_time: " << event.end_time << "\n";

            auto proposed_activity = stg::activity(event.name, event.color);

            auto slots_for_event = _time_slots.slots_in_time_window(event.begin_time,
                                                                    event.end_time);

            auto activity_index = activities().index_of(proposed_activity);
            if (!activity_index) {
                _activities.silently_add(proposed_activity);
                activity_index = _activities.size() - 1;
            }

            auto *activity = activities().at(*activity_index);
            for (auto &slot : slots_for_event) {
                slot->activity = activity;
            }
        }

        _activities.on_change_event();
        time_slots_changed();

        commit_to_history();

        std::cout << "]\n";
    }

#pragma mark - Collections

    auto strategy::activities() const -> const activity_list & {
        return _activities;
    }

    auto strategy::sessions() const -> const sessions_list & {
        return _sessions;
    }

    auto strategy::time_slots() const -> const time_slots_state & {
        return _time_slots;
    }

#pragma mark - Time Grid Properties

    auto stg::strategy::begin_time() const -> time_t {
        return _time_slots.begin_time();
    }

    void strategy::set_begin_time(time_t begin_time) {
        _time_slots.set_begin_time(begin_time);

        commit_to_history();
    }

    auto strategy::time_slot_duration() const -> duration_t {
        return _time_slots.slot_duration();
    }

    void strategy::set_time_slot_duration(duration_t time_slot_duration) {
        _time_slots.set_slot_duration(time_slot_duration);

        commit_to_history();
    }

    auto strategy::end_time() const -> time_t {
        return _time_slots.end_time();
    }

    void strategy::set_end_time(time_t end_time) {
        _time_slots.set_end_time(end_time);

        commit_to_history();
    }

    auto strategy::number_of_time_slots() const -> size_t {
        return _time_slots.number_of_slots();
    }

    auto strategy::duration() const -> duration_t {
        if (time_slots().empty()) {
            return duration_t();
        }

        return time_slots().last().end_time() - time_slots().first().begin_time;
    }

#pragma mark - Real-Time Properties

    auto strategy::active_session() const -> const session * {
        const auto *current_session = get_current_session();
        if (!current_session || !current_session->activity)
            return nullptr;

        return current_session;
    }

    auto strategy::upcoming_session() const -> const session * {
        const auto *current_session = get_current_session();

        if (!current_session) {
            // we're out of strategy's time bounds.
            return nullptr;
        }

        const auto *next_session = sessions().session_after(*current_session);
        return next_session && next_session->activity
                   ? next_session
                   : nullptr;
    }

    auto strategy::progress() const -> float {
        auto current_secs = time_utils::current_seconds();
        auto begin_secs = begin_time() * 60;

        if (end_time() > 24 * 60 && current_secs < begin_secs)
            current_secs += 24 * 3600;

        auto elapsed_secs = current_secs - begin_secs;
        auto duration_secs = duration() * 60;

        if (elapsed_secs < 0)
            elapsed_secs = 0;

        if (elapsed_secs > duration_secs)
            elapsed_secs = duration_secs;

        return (float) elapsed_secs / (float) duration_secs;
    }

    auto strategy::get_current_session() const -> const session * {
        auto it = std::find_if(sessions().begin(),
                               sessions().end(),
                               [](const session &session) {
                                   return session.is_current();
                               });

        if (it != sessions().end()) {
            return &*it;
        }

        return nullptr;
    }

#pragma mark - Operations On Activities

    void strategy::add_activity(const activity &activity) {
        _activities.add(activity);

        commit_to_history();
    }

    void strategy::silently_add_activity(const activity &activity) {
        _activities.silently_add(activity);

        commit_to_history();
    }

    void strategy::delete_activity(activity_index_t activity_index) {
        _time_slots.remove_activity(_activities.at(activity_index));
        _activities.remove_at_index(activity_index);

        commit_to_history();
    }

    void strategy::silently_delete_activity(activity_index_t activity_index) {
        _time_slots.remove_activity(_activities.at(activity_index));
        _activities.silently_remove_at_index(activity_index);

        commit_to_history();
    }

    void strategy::edit_activity(activity_index_t activity_index, const activity &new_activity) {
        auto *old_activity = _activities.at(activity_index);
        _activities.silently_edit_at_index(activity_index, new_activity);

        auto *updated_activity = _activities.at(activity_index);
        _time_slots.edit_activity(old_activity, updated_activity);

        _activities.on_change_event();

        commit_to_history();
    }

    void strategy::silently_edit_activity(activity_index_t activity_index, const activity &new_activity) {
        auto *const old_activity = _activities.at(activity_index);
        _activities.silently_edit_at_index(activity_index, new_activity);

        auto *const updated_activity = _activities.at(activity_index);
        _time_slots.edit_activity(old_activity, updated_activity);

        commit_to_history();
    }

    void strategy::drag_activity(activity_index_t from_index, activity_index_t to_index) {
        _activities.drag(from_index, to_index);

        commit_to_history();
    }

    void strategy::silently_drag_activity(activity_index_t from_index, activity_index_t to_index) {
        _activities.silently_drag(from_index, to_index);

        commit_to_history();
    }

    void strategy::reorder_activities_by_usage() {
        std::map<activity *, duration_t> usage;

        for (const auto &session : sessions()) {
            if (session.activity == no_activity)
                continue;

            usage[session.activity] += session.duration();
        }

        std::vector<std::pair<activity *, duration_t>> pairs;
        pairs.reserve(usage.size());

        for (const auto &elem : usage) {
            pairs.emplace_back(elem);
        }

        std::sort(pairs.begin(), pairs.end(), [=](auto &a, auto &b) {
            return a.second > b.second;
        });

        activity_list::data_t reordered;
        for (auto &elem : pairs) {
            if (!elem.first)
                continue;

            auto index = *activities().index_of(elem.first);
            auto &activity = _activities._data[index];

            reordered.push_back(activity);
        }

        for (const auto &activity : _activities) {
            if (std::find_if(reordered.begin(), reordered.end(), [&activity](auto &a) {
                    return a == activity;
                }) == reordered.end()) {
                reordered.push_back(activity);
            };
        }

        if (_activities._data != reordered) {
            _activities.reset_with(reordered);
            _activities.on_change_event();

            commit_to_history();
        }
    }

#pragma mark - Operations On Slots

    void strategy::place_activity(activity_index_t activity_index,
                                  const std::vector<time_slot_index_t> &time_slot_indices) {
        if (!activities().has_index(activity_index))
            return;

        auto *activity = activities().at(activity_index);
        _time_slots.set_activity_at_indices(activity, time_slot_indices);

        commit_to_history();
    }

    void strategy::make_empty_at(const std::vector<time_slot_index_t> &time_slot_indices) {
        _time_slots.set_activity_at_indices(no_activity, time_slot_indices);

        commit_to_history();
    }

    void strategy::shift_below_time_slot(time_slot_index_t from_index, int length) {
        _time_slots.shift_below(from_index, length);

        commit_to_history();
    }

    auto strategy::is_resizing() const -> bool {
        return current_resize_operation != nullptr;
    }

    void strategy::begin_resizing() {
        current_resize_operation = std::make_unique<resize_operation>(&_time_slots);
    }

    void strategy::fill_time_slots_shifting(time_slot_index_t from_index, time_slot_index_t till_index) {
        assert("fill_time_slots must be called between begin_resizing() and end_resizing() calls" &&
               current_resize_operation);

        current_resize_operation->fill_slots_shifting(from_index, till_index);
    }

    void strategy::fill_time_slots(time_slot_index_t from_index, time_slot_index_t till_index) {
        assert("fill_time_slots must be called between begin_resizing() and end_resizing() calls" &&
               current_resize_operation);

        current_resize_operation->fill_slots(from_index, till_index);
    }

    void strategy::end_resizing() {
        assert(current_resize_operation && "end_resizing must be called after begin_resizing()");

        if (current_resize_operation->state_changed()) {
            commit_to_history();
        }

        current_resize_operation.reset();
    }

    auto strategy::is_dragging() const -> bool {
        return current_drag_operation != nullptr;
    }

    void strategy::begin_dragging(session_index_t session_index) {
        const auto &session = sessions()[session_index];
        auto initial_indices = global_slot_indices_from_session(session);

        current_drag_operation = std::make_unique<drag_operation>(&_time_slots, initial_indices);
    }

    auto strategy::drag_session(session_index_t session_index,
                                int distance) -> sessions_list::index_t {
        assert(current_drag_operation && "drag_session must be called between "
                                         "begin_dragging() and end_dragging() calls");

        if (session_index < 0 ||
            session_index > sessions().size() - 1 ||
            distance == 0) {
            return session_index;
        }

        const auto &session = sessions()[session_index];
        if (session.activity == stg::strategy::no_activity) {
            return session_index;
        }

        auto new_indexes = current_drag_operation->record_drag(session.time_slots, distance);

        time_slots().on_change_event();

        auto new_session_index = new_indexes.empty()
                                     ? session_index
                                     : sessions().session_index_for_time_slot_index(new_indexes.front());

        return new_session_index;
    }

    auto strategy::global_slot_indices_from_session(const session &session) const -> std::vector<time_slot_index_t> {
        drag_operation::indices_vector initial_indices;
        std::transform(session.time_slots.begin(),
                       session.time_slots.end(),
                       std::back_inserter(initial_indices),
                       [this](auto &slot) -> time_slots_state::index_t {
                           return *_time_slots.index_of(slot);
                       });

        return initial_indices;
    }

    void strategy::end_dragging() {
        assert(current_drag_operation && "end_dragging must be called after begin_dragging()");

        if (current_drag_operation->state_changed()) {
            commit_to_history();
        }

        current_drag_operation.reset();
    }

    void strategy::cancel_dragging() {
        _time_slots.reset_with(current_drag_operation->initial_state());
        current_drag_operation.reset();

        time_slots_changed();
    }

    void strategy::copy_session(session_index_t session_index, time_slot_index_t begin_index) {
        const auto &session = sessions()[session_index];
        if (!session.activity) {
            return;
        }

        auto copied_session_indices = std::vector<time_slot_index_t>(session.length());
        std::iota(copied_session_indices.begin(),
                  copied_session_indices.end(),
                  begin_index);

        _time_slots.set_activity_at_indices(session.activity, copied_session_indices);

        commit_to_history();
    }

    void strategy::copy_slots(time_slot_index_t from_index,
                              time_slot_index_t till_index,
                              time_slot_index_t destination_index) {
        _time_slots.copy_slots(from_index, till_index, destination_index);

        commit_to_history();
    }

#pragma mark - History

    void strategy::commit_to_history() {
        if (history.commit(make_history_entry()))
            on_change_event();
    }

    void strategy::undo() {
        auto history_entry = history.undo();
        if (history_entry) {
            apply_history_entry(history_entry);

            on_change_event();
        }
    }

    void strategy::redo() {
        auto history_entry = history.redo();
        if (history_entry) {
            apply_history_entry(history_entry);

            on_change_event();
        }
    }

    auto strategy::has_undo() -> bool {
        return history.has_prevoius_state();
    }

    auto strategy::has_redo() -> bool {
        return history.has_next_state();
    }

    auto strategy::has_activities_undo() -> bool {
        return history.has_prevoius_activities_state();
    }

    auto strategy::has_activities_redo() -> bool {
        return history.has_next_activities_state();
    }

    auto strategy::make_history_entry() -> strategy_history::entry {
        return strategy_history::entry{_activities.data(), _time_slots.data()};
    }

    void strategy::apply_history_entry(const std::optional<strategy_history::entry> &history_entry) {
        _activities.reset_with(history_entry->activities);
        _time_slots.reset_with(history_entry->time_slots);

        _time_slots.on_change_event();
        _activities.on_change_event();
    }

#pragma mark - Handling Sate Changes

    void strategy::time_slots_changed() {
        _sessions.recalculate(_time_slots);
    }

    void strategy::setup_time_slots_callback() {
        _time_slots.add_on_change_callback(this, &strategy::time_slots_changed);
    }

}
