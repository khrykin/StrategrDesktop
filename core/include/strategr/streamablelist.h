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

        friend std::ostream &operator<<(std::ostream &os, const Container &list) {
            os << list.class_print_name() << " [" << std::endl;
            for (auto &element : list) {
                os << "\t" << element;
                os << std::endl;
            }

            os << "]";
            return os;
        }

        friend std::ostream &operator<<(std::ostream &os, const Container *list) {
            return operator<<(os, *list);
        }

    protected:
        virtual std::string class_print_name() const = 0;
    };
};

#endif//MODELS_TESTS_STREAMABLELIST_H
