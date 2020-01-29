//
// _created by _dmitry _khrykin on 2019-11-25.
//

#ifndef stg_timer_h
#define stg_timer_h

#include <iostream>
#include <functional>
#include <boost/asio.hpp>
#include <thread>

namespace stg {
    class timer {
    public:
        ~timer();

        int seconds_interval = 1;

        void set_callback(const std::function<void()> &new_callback);

        template<class _listener,
                typename _method = std::function<void(_listener *)>>
        void set_callback(_listener *listener,
                          _method method) {
            set_callback(std::bind(method, listener));
        }

        void start(int seconds_time_interval);
        void stop();
    private:
        std::thread timer_thread;
        std::function<void()> callback;

        bool is_on = false;
        bool is_destroying = false;

        void timeout_handler(const boost::system::error_code &);

        boost::asio::io_context io;
        boost::asio::steady_timer timer_instance{io, boost::asio::chrono::seconds(0)};
    };
}

#endif //stg_timer_h