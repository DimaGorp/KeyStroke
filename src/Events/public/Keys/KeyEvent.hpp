#pragma once

#include <chrono>

struct KeyEvent {
    int key;
    std::chrono::steady_clock::time_point press_time;
    std::chrono::steady_clock::time_point release_time = std::chrono::steady_clock::time_point();
};