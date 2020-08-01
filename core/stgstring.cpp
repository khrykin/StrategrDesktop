//
// Created by Dmitry Khrykin on 15.06.2020.
//

#include <sstream>

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "stgstring.h"

namespace stg {

    auto make_string_uuid() -> std::string {
        auto uuid = boost::uuids::random_generator()();

        std::stringstream sstream;
        sstream << uuid;

        return sstream.str();
    }

}