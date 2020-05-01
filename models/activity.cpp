#include <iostream>
#include <algorithm>

#include "activity.h"
#include "activityinvalidpropertyexception.h"

namespace stg {
    auto activity::default_colors() -> const std::vector<stg::activity::color_info_t> & {
        const static std::vector<activity::color_info_t> colors = {
                {"#FF6562", "Red"},
                {"#FFB700", "Orange"},
                {"#FFD600", "Yellow"},
                {"#A463F2", "Purple"},
                {"#D5008F", "Indigo"},
                {"#19A974", "Green"},
                {"#357EDD", "Blue"},
                {"#000000", "Black"},
                {"#777777", "Gray"}
        };

        return colors;
    };

    activity::activity(name_t name, color_t color) : _color(std::move(color)) {
        if (!is_valid(name)) {
            throw empty_name_exception();
        }

        _name = std::move(name);
    }

    auto activity::is_valid(const activity::name_t &name) -> bool {
        bool white_spaces_only = name.find_first_not_of(" \t\n\v\f\r") == std::string::npos;
        return !white_spaces_only;
    }

    auto activity::empty_name_exception() -> activity::invalid_property_exception {
        const auto *message = "activity name can't be empty";
        return invalid_property_exception(message);
    }

    auto activity::name() const -> const activity::name_t & {
        return _name;
    }

    auto activity::color() const -> const activity::color_t & {
        return _color;
    }

    auto operator==(const stg::activity &lhs, const stg::activity &rhs) -> bool {
        return lhs.name() == rhs.name() &&
               lhs.color() == rhs.color();
    }

    auto operator!=(const stg::activity &lhs, const stg::activity &rhs) -> bool {
        return !(lhs == rhs);
    }

    auto operator<<(std::ostream &os,
                    const stg::activity &activity) -> std::ostream & {
        os << "activity("
           << activity.name()
           << ", "
           << activity.color()
           << ")";

        return os;
    }

    auto activity::with_name(const name_t &name) const -> activity {
        return activity(name, _color);
    }

    auto activity::with_color(const color_t &color) const -> activity {
        return activity(_name, color);
    }

    auto activity::to_json() const -> nlohmann::json {
        nlohmann::json j;
        j[keys::name] = this->name();
        j[keys::color] = this->color();
        return j;
    }

    auto activity::from_json(const nlohmann::json &j) -> activity {
        auto name = j[keys::name];

        stg::color color = activity::default_color;
        if (j.count(keys::color) && !j[keys::color].is_null()) {
            color = j[keys::color];
        }

        return activity{name, color};
    }
}