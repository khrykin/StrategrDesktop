//
// Created by Dmitry Khrykin on 2019-07-06.
//

#ifndef MODELS_TESTS_STREAMABLELIST_H
#define MODELS_TESTS_STREAMABLELIST_H

#include <iostream>

template<class ContainerType>
class StreamableList {
public:
    virtual ~StreamableList() = default;

    friend std::ostream &operator<<(std::ostream &os, const ContainerType &list) {
        os << list.classPrintName() << " [" << std::endl;
        for (auto &element : list) {
            os << "\t" << element;
            os << std::endl;
        }

        os << "]";
        return os;
    }

protected:
    virtual std::string classPrintName() const = 0;
};

#endif //MODELS_TESTS_STREAMABLELIST_H
