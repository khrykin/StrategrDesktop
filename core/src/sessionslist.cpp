//
// created by dmitry khrykin on 2019-07-06.
//

#include "sessionslist.h"
#include "activity.h"
#include <algorithm>

namespace stg {

    auto sessions_list::session_index_for_time_slot_index(index_t time_slot_index) const -> index_t {
        index_t slots_count = 0;

        for (const auto &session : _data) {
            auto first_slot_index = slots_count;
            auto next_slot_index = slots_count + static_cast<index_t>(session.length());

            if (first_slot_index <= time_slot_index && time_slot_index < next_slot_index) {
                return static_cast<index_t>(&session - &_data[0]);
            }

            slots_count = next_slot_index;
        }

        return -1;
    }

    void sessions_list::recalculate(const time_slots_state &time_slots) {
        std::vector<session> result;

        auto cached_session = session();

        for (const auto &time_slot : time_slots) {
            auto time_slot_index = &time_slot - &time_slots[0];
            auto *previous_activity = time_slot_index > 0
                                          ? time_slots[(index_t) time_slot_index - 1].activity
                                          : time_slot::no_activity;

            auto *current_activity = time_slot.activity;

            bool activity_changed = previous_activity != current_activity;

            auto default_session = session{{time_slot}, time_slot.activity};

            if (time_slot_index == 0 || activity_changed) {
                if (activity_changed && time_slot_index != 0)
                    result.push_back(cached_session);

                cached_session = default_session;
            } else {
                cached_session.time_slots.push_back(time_slot);
            }

            if (time_slot_index == time_slots.number_of_slots() - 1) {
                result.push_back(cached_session);
            }
        }

        _data = result;

        on_change_event();
    }

    auto sessions_list::session_after(const session &activity_session) const -> const session * {
        auto it = find_const(activity_session);
        return it < std::prev(_data.end()) ? &*std::next(it) : nullptr;
    }

    auto sessions_list::session_before(const session &activity_session) const -> const session * {
        auto it = find_const(activity_session);
        return it > _data.begin() ? &*std::prev(it) : nullptr;
    }

    auto sessions_list::class_print_name() const -> std::string {
        return "sessions_list";
    }

    auto sessions_list::overview() const -> std::vector<overview_item> {
        if (_data.empty())
            return {};

        auto overall_begin_time = _data.front().begin_time();

        std::vector<overview_item> result;
        auto make_overview_item = [overall_begin_time, this](auto &session) {
            auto duration_percentage = (float) session.duration() / duration();
            auto begin_percentage = (float) (session.begin_time() - overall_begin_time) / duration();

            auto color = session.activity
                             ? std::make_optional(session.activity->color())
                             : std::nullopt;

            return overview_item{duration_percentage,
                                 begin_percentage,
                                 color};
        };

        std::transform(_data.begin(), _data.end(),
                       std::back_inserter(result), make_overview_item);

        return result;
    }

    auto sessions_list::duration() const -> time_slot::minutes {
        if (_data.empty()) return 0;
        return _data.back().end_time() - _data.front().begin_time();
        ;
    }

    auto sessions_list::get_non_empty() const -> std::vector<session> {
        std::vector<session> result;
        std::copy_if(_data.begin(), _data.end(),
                     std::back_inserter(result), [](auto &session) {
                         return session.activity != time_slot::no_activity;
                     });

        return result;
    }

    auto sessions_list::relative_begin_time(const session &session) const -> time_slot::minutes {
        return session.begin_time() - _data.front().begin_time();
    }

    auto sessions_list::get_bounds_for(index_t session_index) const -> bounds {
        const auto &session = _data[session_index];

        auto global_begin_time = _data.front().begin_time();
        auto slot_duration = static_cast<index_t>(session.time_slots.front().duration);

        auto start_index = static_cast<index_t>(session.begin_time() - global_begin_time) / slot_duration;
        auto end_index = start_index + static_cast<index_t>(session.length());

        return {start_index, end_index};
    }
}
