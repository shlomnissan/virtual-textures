// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <chrono>
#include <format>
#include <iostream>

using Clock = std::chrono::steady_clock;

class Timer {
public:
    Timer() : start_time_(Clock::now()) {};

    auto GetMilliseconds() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            Clock::now() - start_time_
        ).count();
    }

    auto GetSeconds() const {
        return std::chrono::duration_cast<std::chrono::duration<double>>(
            Clock::now() - start_time_
        ).count();
    }

    auto Reset() { start_time_ = Clock::now(); }

    auto Print() const { std::cout << std::format("{}ms\n", GetMilliseconds()); }

private:
    std::chrono::time_point<Clock> start_time_;
};