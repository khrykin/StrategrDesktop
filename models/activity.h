#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <string>

struct Activity {
    using Color = std::string;
    using Name = std::string;
    class InvalidPropertyException;

    static constexpr auto defaultColor = "#000000";

    explicit Activity(Name name, Color color = defaultColor);

    const Name &name() const;
    const Color &color() const;

    Activity copyChangingName(const Name &name) const;
    Activity copyChangingColor(const Color &color) const;

    friend bool operator==(const Activity &lhs, const Activity &rhs);
    friend bool operator!=(const Activity &lhs, const Activity &rhs);

    friend std::ostream &operator<<(std::ostream &os,
                                    const Activity &activity);

private:
    Name _name;
    Color _color;

    static InvalidPropertyException emptyNameException();

    static bool isValid(const Name &name);
};

#endif // ACTIVITY_H
