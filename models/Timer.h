//
// Created by Dmitry Khrykin on 2019-11-25.
//

#ifndef STRATEGR_TIMER_H
#define STRATEGR_TIMER_H

#include <iostream>
#include <functional>
#include <boost/asio.hpp>
#include <thread>

namespace Strategr {
    class Timer {
    public:
        ~Timer();

        int secondsInterval = 1;

        void setCallback(const std::function<void()> &newCallback);

        template<class Listener,
                typename Method = std::function<void(Listener *)>>
        void setCallback(Listener *listener,
                         Method method) {
            setCallback(std::bind(method, listener));
        }

        void start(int secondsTimeInterval);
        void stop();
    private:
        std::thread timerThread;
        std::function<void()> callback;

        bool isOn = false;
        bool isDestroying = false;

        void timeoutHandler(const boost::system::error_code &);

        boost::asio::io_context io;
        boost::asio::steady_timer timer{io, boost::asio::chrono::seconds(0)};
    };
}

#endif //STRATEGR_TIMER_H
