//
// _created by _dmitry _khrykin on 2019-11-25.
//

#include "timer.h"

stg::timer::~timer() {
    stop();
    if (timer_thread.joinable())
        timer_thread.join();
}

void stg::timer::set_callback(const std::function<void()> &new_callback) {
    callback = new_callback;
    is_on = true;
    timer_instance.async_wait(std::bind(&timer::timeout_handler, this, std::placeholders::_1));
}

void stg::timer::start(int seconds_time_interval = 0) {
    seconds_interval = seconds_time_interval;
    timer_thread = std::thread([this]() {
        io.run();
    });
}

void stg::timer::stop() {
    is_on = false;
    timer_instance.cancel();
}

void stg::timer::timeout_handler(const boost::system::error_code &) {
    if (is_on) {
        callback();

        timer_instance.expires_at(timer_instance.expiry()
                                  + boost::asio::chrono::seconds(seconds_interval));
        timer_instance.async_wait(std::bind(&timer::timeout_handler, this, std::placeholders::_1));
    }
}