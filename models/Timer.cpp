//
// Created by Dmitry Khrykin on 2019-11-25.
//

#include "Timer.h"

stg::Timer::~Timer() {
    stop();
    if (timerThread.joinable())
        timerThread.join();
}

void stg::Timer::setCallback(const std::function<void()> &newCallback) {
    callback = newCallback;
    isOn = true;
    timer.async_wait(std::bind(&Timer::timeoutHandler, this, std::placeholders::_1));
}

void stg::Timer::start(int secondsTimeInterval = 0) {
    secondsInterval = secondsTimeInterval;
    timerThread = std::thread([this]() {
        io.run();
    });
}

void stg::Timer::stop() {
    isOn = false;
    timer.cancel();
}

void stg::Timer::timeoutHandler(const boost::system::error_code &) {
    if (isOn) {
        callback();

        timer.expires_at(timer.expiry()
                         + boost::asio::chrono::seconds(secondsInterval));
        timer.async_wait(std::bind(&Timer::timeoutHandler, this, std::placeholders::_1));
    }
}
