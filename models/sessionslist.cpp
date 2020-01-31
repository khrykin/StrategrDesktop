//
// created by dmitry khrykin on 2019-07-06.
//

#include <algorithm>
#include "sessionslist.h"

stg::sessions_list::index_t
stg::sessions_list::session_index_for_time_slot_index(index_t time_slot_index) const {
    auto slots_count = 0;
    for (auto &session : _data) {
        auto first_slot_index = slots_count;
        auto next_slot_index = slots_count + session.length();

        if (first_slot_index <= time_slot_index &&
            time_slot_index < next_slot_index) {
            return static_cast<index_t>(&session - &_data[0]);
        }

        slots_count = next_slot_index;
    }

    return -1;
}

void stg::sessions_list::recalculate(const time_slots_state &time_slots_state) {
    _data = sessions_calculator::calculate(time_slots_state);

    on_change_event();
}

const stg::session *
stg::sessions_list::session_after(const session &activity_session) const {
    auto it = find_const(activity_session);
    return it < std::prev(_data.end()) ? &*std::next(it) : nullptr;
}

std::optional<stg::session>
stg::sessions_list::session_before(const session &activity_session) const {
    auto it = find_const(activity_session);
    return it > _data.begin()
           ? std::make_optional(*std::prev(it))
           : std::nullopt;
}

std::string stg::sessions_list::class_print_name() const {
    return "sessions_list";
}

std::vector<stg::sessions_list::overview_item>
stg::sessions_list::overview() const {
    if (_data.empty()) {
        return {};
    }

    auto overall_begin_time = _data.front().begin_time();
    auto total_duraion = _data.back().end_time() - overall_begin_time;

    std::vector<overview_item> result;
    std::transform(_data.begin(),
                   _data.end(),
                   std::back_inserter(result),
                   [total_duraion, overall_begin_time](auto &session) {
                       auto duration_percentage = static_cast<float>(session.duration())
                                                  / total_duraion;
                       auto begin_percentage = static_cast<float>(session.begin_time()
                                                                  - overall_begin_time)
                                               / total_duraion;

                       auto color = session.activity
                                    ? std::make_optional(session.activity->color())
                                    : std::nullopt;

                       return overview_item{
                               duration_percentage,
                               begin_percentage,
                               color
                       };
                   });

    return result;
}

std::vector<stg::session> stg::sessions_list::get_non_empty() const {
    std::vector<session> result;
    std::copy_if(_data.begin(),
                 _data.end(),
                 std::back_inserter(result),
                 [](auto &session) {
                     return session.activity != time_slot::no_activity;
                 });

    return result;
}

stg::time_slot::time_t stg::sessions_list::relative_time(const stg::session &session) const {
    return session.begin_time() - _data.front().begin_time();
}
