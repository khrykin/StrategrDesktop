#include <algorithm>
#include <vector>
#include <map>

#include "strategy.h"
#include "json.h"
#include "time_utils.h"

stg::strategy::strategy(time_t begin_time, duration_t time_slot_duration,
                        size_t number_of_time_slots)
        : _time_slots(time_slots_state(begin_time,
                                       time_slot_duration,
                                       number_of_time_slots)),

          history(make_history_entry()) {
    time_slots_changed();
    setup_time_slots_callback();
}

stg::strategy::strategy(const time_slots_state::data_t &time_slots,
                        const activity_list::data_t &activities) :
        _time_slots(time_slots),
        _activities(activities),
        history(make_history_entry()) {
    time_slots_changed();
    setup_time_slots_callback();
}

stg::strategy::strategy(const strategy &other) :
        _time_slots(other._time_slots.data()),
        _activities(other._activities.data()),
        _sessions(other.sessions().data()),
        history(make_history_entry()) {

    setup_time_slots_callback();
}

stg::strategy &stg::strategy::operator=(const strategy &other) {
    _time_slots.reset_with(other.time_slots().data());
    // Note: _time_slots on_change callback has_only private,
    // so we don't call on_change_event();

    _activities.reset_with(other.activities().data());
    _activities.on_change_event();

    _sessions.reset_with(other.sessions().data());
    _sessions.on_change_event();

    history = strategy_history(make_history_entry());

    return *this;
}

const stg::activity_list &stg::strategy::activities() const {
    return _activities;
}

void stg::strategy::add_activity(const activity &activity) {
    _activities.add(activity);
    commit_to_history();
}

void stg::strategy::delete_activity(activity_index activity_index) {
    _time_slots.remove_activity(_activities.at(activity_index));
    _activities.remove_at_index(activity_index);

    commit_to_history();
}

void stg::strategy::edit_activity(activity_index activity_index,
                                  const activity &new_activity) {
    const auto old_activity = _activities.at(activity_index);
    _activities.edit_at_index(activity_index, new_activity);

    const auto updated_activity = _activities.at(activity_index);
    _time_slots.edit_activity(old_activity, updated_activity);

    commit_to_history();
}

stg::strategy::time_t stg::strategy::begin_time() const {
    return _time_slots.begin_time();
}

void stg::strategy::set_begin_time(time_t begin_time) {
    _time_slots.set_begin_time(begin_time);

    commit_to_history();
}

stg::strategy::duration_t stg::strategy::time_slot_duration() const {
    return _time_slots.slot_duration();
}

void stg::strategy::set_time_slot_duration(duration_t time_slot_duration) {
    _time_slots.set_slot_duration(time_slot_duration);

    commit_to_history();
}

stg::strategy::size_t stg::strategy::number_of_time_slots() const {
    return _time_slots.number_of_slots();
}

void stg::strategy::set_number_of_time_slots(size_t number_of_time_slots) {
    _time_slots.set_number_of_slots(number_of_time_slots);

    commit_to_history();
}

void stg::strategy::time_slots_changed() {
    _sessions.recalculate(_time_slots);
}

void stg::strategy::place_activity(activity_index activity_index,
                                   const std::vector<time_slot_index_t> &time_slot_indices) {
    if (!activities().has_index(activity_index)) {
        return;
    }

    auto activity = activities().at(activity_index);
    _time_slots.set_activity_at_indices(activity, time_slot_indices);

    commit_to_history();
}

void stg::strategy::make_empty_at(const std::vector<time_slot_index_t> &time_slot_indices) {
    _time_slots.set_activity_at_indices(stg::strategy::no_activity, time_slot_indices);

    commit_to_history();
}

void stg::strategy::drag_activity(activity_index from_index, activity_index to_index) {
    _activities.drag(from_index, to_index);
}

void stg::strategy::fill_time_slots(time_slot_index_t from_index, time_slot_index_t till_index) {
    assert(current_resize_operation && "fill_time_slots must be called between "
                                       "begin_resizing() and end_resizing() calls");

    current_resize_operation->fill_slots(from_index, till_index);
}

const stg::sessions_list &stg::strategy::sessions() const {
    return _sessions;
}

stg::strategy::time_t stg::strategy::end_time() const {
    return _time_slots.last().end_time();
}

void stg::strategy::setup_time_slots_callback() {
    _time_slots.add_on_change_callback(this, &stg::strategy::time_slots_changed);
}

stg::strategy_history::entry stg::strategy::make_history_entry() {
    return strategy_history::entry{_activities.data(),
                                   _time_slots.data()};
}

void stg::strategy::commit_to_history() {
    history.commit(make_history_entry());

    on_change_event();
}

void stg::strategy::undo() {
    auto history_entry = history.undo();
    if (history_entry) {
        apply_history_entry(history_entry);

        on_change_event();
    }
}

void stg::strategy::redo() {
    auto history_entry = history.redo();
    if (history_entry) {
        apply_history_entry(history_entry);

        on_change_event();
    }
}

void stg::strategy::apply_history_entry(const std::optional<strategy_history::entry> &history_entry) {
    _activities.reset_with(history_entry->activities);
    _activities.on_change_event();

    _time_slots.reset_with(history_entry->time_slots);
    _time_slots.on_change_event();
}

const stg::time_slots_state &stg::strategy::time_slots() const {
    return _time_slots;
}

void stg::strategy::shift_below_time_slot(stg::strategy::time_slot_index_t from_index, int length) {
    _time_slots.shift_below(from_index, length);

    commit_to_history();
}

void stg::strategy::drag_session(stg::strategy::session_index_t session_index,
                                 int distance) {
    assert(current_drag_operation && "drag_session must be called between "
                                     "begin_dragging() and end_dragging() calls");


    if (session_index < 0 ||
        session_index > sessions().size() - 1 ||
        distance == 0) {
        return;
    }

    const auto session = sessions()[session_index];
    if (session.activity == stg::strategy::no_activity) {
        return;
    }


    current_drag_operation->record_drag(session.time_slots, distance);

    time_slots_changed();
}

void stg::strategy::begin_dragging(session_index_t session_index) {
    auto &session = sessions()[session_index];
    auto initial_indices = global_slot_indices_from_session(session);

    current_drag_operation = std::make_shared<drag_operation>(&_time_slots, initial_indices);
}

std::vector<stg::strategy::time_slot_index_t>
stg::strategy::global_slot_indices_from_session(const session &session) const {
    drag_operation::indices_vector initial_indices;
    std::transform(session.time_slots.begin(),
                   session.time_slots.end(),
                   std::back_inserter(initial_indices),
                   [this](auto &slot) -> time_slots_state::index_t {
                       return *_time_slots.index_of(slot);
                   });

    return initial_indices;
}

void stg::strategy::end_dragging() {
    if (!current_drag_operation) {
        return;
    }

    if (current_drag_operation->state_changed()) {
        commit_to_history();
    }

    current_drag_operation.reset();
}

void stg::strategy::begin_resizing() {
    current_resize_operation = std::make_shared<resize_operation>(&_time_slots);
}

void stg::strategy::end_resizing() {
    if (!current_resize_operation) {
        return;
    }

    if (current_resize_operation->state_changed()) {
        commit_to_history();
    }

    current_resize_operation.reset();
}

stg::strategy::duration_t stg::strategy::duration() const {
    if (time_slots().empty()) {
        return duration_t();
    }

    return time_slots().last().end_time() - time_slots().first().begin_time;
}

std::string stg::strategy::to_json_string() const {
    return json::serialize(*this);
}

std::unique_ptr<stg::strategy> stg::strategy::from_json_string(const std::string &json_string) {
    return json::parse(json_string);
}

const stg::session *stg::strategy::get_current_session() const {
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

const stg::session *stg::strategy::get_active_session() const {
    auto current_session = this->get_current_session();
    if (!current_session || !current_session->activity) {
        return nullptr;
    }

    return current_session;
}

const stg::session *stg::strategy::upcoming_session() const {
    auto current_session = this->get_current_session();

    if (!current_session) {
        // we're out of strategy's time bounds.
        return nullptr;
    }

    const auto next_session = sessions().session_after(*current_session);
    return next_session && next_session->activity
           ? next_session
           : nullptr;
}

void stg::strategy::reorder_activities_by_usage() {
    std::map<activity *, duration_t> usage;

    for (auto &session : sessions()) {
        if (session.activity == no_activity)
            continue;

        usage[session.activity] += session.duration();
    }

    std::vector<std::pair<activity *, duration_t>> pairs;
    for (const auto &elem : usage) {
        pairs.emplace_back(elem);
    }

    std::sort(pairs.begin(),
              pairs.end(),
              [=](auto &a, auto &b) {
                  return a.second > b.second;
              });

    std::vector<std::shared_ptr<activity>> reordered;
    for (auto &elem : pairs) {
        if (!elem.first)
            continue;

        auto index = *activities().index_of(elem.first);
        auto activity = _activities._data[index];

        reordered.push_back(activity);
    }

    for (auto &activity : _activities) {
        if (std::find_if(reordered.begin(),
                         reordered.end(),
                         [activity](auto a) {
                             return a == activity;
                         }) == reordered.end()) {

            reordered.push_back(activity);
        };
    }

    _activities.reset_with(reordered);
    _activities.on_change_event();

    commit_to_history();
}