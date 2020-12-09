//
// Created by Dmitry Khrykin on 2019-07-06.
//

#ifndef STREAMABLELIST_H
#define STREAMABLELIST_H

#include <iostream>

namespace stg {
    template<class Container>
    class streamable_list {
    public:
        virtual ~streamable_list() = default;

        friend auto operator<<(std::ostream &os, const Container &list) -> std::ostream & {
            os << list.class_print_name() << " [" << std::endl;
            for (auto &element : list) {
                os << "\t" << element;
                os << std::endl;
            }

            os << "]";
            return os;
        }

        friend auto operator<<(std::ostream &os, const Container *list) -> std::ostream & {
            return operator<<(os, *list);
        }

    protected:
        virtual auto class_print_name() const -> std::string = 0;
    };
}

#endif//MODELS_TESTS_STREAMABLELIST_H
